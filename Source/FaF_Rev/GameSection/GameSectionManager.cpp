// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSectionManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelScriptActor.h"
#include "GameSection/GameSectionNode.h"
#include "GameSection/GameSectionGraph.h"
#include "Libraries/GTLoadUtilsLibrary.h"
#include "Inventory/InventoryItemData.h"
#include "SaveSystem/SaveSubsystem.h"
#include "Screens/LoadingWidget.h"
#include "NarrativeComponent.h"
#include "PlayerTeleporter.h"
#include "FRGameState.h"
#include "FRGameMode.h"
#include "FRSettings.h"
#include "FaF_Rev.h"

void UGameSectionManager::LoadSequence()
{
	SaveSystem->LoadGameData();
	SetSequence(SaveSystem->GetGameDataObject()->Sequence);
}

void UGameSectionManager::SetSequence(const TArray<uint8>& InSequence)
{
	if (!SectionGraph)
	{
		SMART_LOG(Error, TEXT("Attempting to Transition with a null Section Graph."));
		return;
	}
	
	if (IsLoading())
	{
		SMART_LOG(Warning, TEXT("Game Section Manager is busy, the Transition request will be ignored."));
		return;
	}

	const TArray<uint8> FixedSequence = SectionGraph->ValidateSequence(InSequence);
	if (Sequence != FixedSequence)
	{
		Sequence = FixedSequence;
		BeginTransition();
	}
}

void UGameSectionManager::Step(const uint8 Index)
{
	TArray<uint8> PostStep = Sequence;
	PostStep.Add(PostStep.IsEmpty() ? 0 : Index);
	SetSequence(PostStep);
}

void UGameSectionManager::SaveCurrentTime()
{
	SaveSystem->SaveGameData(PlayTime);
	PlayTime = 0.0f;
}

void UGameSectionManager::BeginTransition()
{
	bLoading = true;
	PlayerChar->FadeToBlack(0.25f);
	PlayerChar->AddLockFlag(Player::LockFlags::Loading);
	
	UGameSectionNode* Node = SectionGraph->GetNodeBySequence<UGameSectionNode>(Sequence);
	if (!Node || !Node->IsA(UGameSectionDataNode::StaticClass()))
	{
		SMART_LOG(Error, TEXT("Cannot begin transition. Node or sequence is invalid."))
		return;
	}
	LastData = ThisData;
	ThisData = Cast<UGameSectionDataNode>(Node);
	
	SaveSystem->GetGameDataObject()->Sequence = Sequence;
	SaveSystem->GetGameDataObject()->Inventory = PlayerChar->GetGameMode()->Inventory->ExportSaveData();
	SaveCurrentTime();
	
	if (ThisData)
	{
		UGlobalSaveObject* GlobalObject = SaveSystem->GetGlobalDataObject();
		GlobalObject->SectionNodes.Add(ThisData->UniqueID);
		GlobalObject->Menus.Add(ThisData->UnlockMenu);
		SaveSystem->SaveGlobalData();
	}

	ShowLoadingWidget();
	
	FTimerHandle Handle;
	if (LastData)
	{
		GetWorld()->GetTimerManager().SetTimer(Handle, this,
			&UGameSectionManager::UnloadLastData, 0.25f, false);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(Handle, this,
			&UGameSectionManager::LoadCurrentData, 0.25f, false);
	}
}

void UGameSectionManager::UnloadLastData()
{
	PlayerChar->ResetStates();
	PlayerChar->TeleportPlayer(FVector::ZeroVector, FRotator::ZeroRotator);
	PlayerChar->GetGameMode()->Narrative->ForgetQuest(LastData->Quest.LoadSynchronous());
	PlayerChar->GetGameState()->StopGameMusic();
	
	LoadedObjs.Empty(ThisData ? ThisData->PreloadObjects.Num() : 0);
	UnloadingLevels = 0;
	bool bHasMaps = false;
	if (LastData)
	{
		GetWorld()->GetLevelScriptActor()->RemoteEvent(TEXT("OnSectionUnload"));
		for (const TPair<TSoftObjectPtr<UWorld>, bool>& Pair : LastData->Levels)
		{
			if (UnloadLevel(Pair.Key)) bHasMaps = true;
		}
	}
	
	if (!bHasMaps)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this,
			&UGameSectionManager::LoadCurrentData, 0.1f, false);
	}
}

void UGameSectionManager::LoadCurrentData()
{
	UGTLoadUtilsLibrary::ForceGarbageCollection();
	checkf(ThisData, TEXT("Trying to load without any valid data."))

	IUDSInterface::SetCustomSettings(PlayerChar->UltraDynamicSky, ThisData->SkyWeatherSettings);
	if (ULoadingWidgetBase* Widget = GetLoadingWidget())
		Widget->bUnloading = false;

	LoadingLevels = 0;
	bool bHasMaps = false;
	if (ThisData)
	{
		for (const TPair<TSoftObjectPtr<UWorld>, bool>& Pair : ThisData->Levels)
		{
			if (LoadLevel(Pair)) bHasMaps = true;
		}

		// For the sake of simplicity, synchronously load these.
		// They should be light enough to not do any blocking loads.
		for (TSoftObjectPtr<UObject>& Obj : ThisData->PreloadObjects)
		{
			LoadedObjs.Add(Obj.LoadSynchronous());
		}
	}
	
	if (!bHasMaps)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this,
			&UGameSectionManager::FinishLoading, 0.1f, false);
	}
}

void UGameSectionManager::FinishLoading()
{
	UGTLoadUtilsLibrary::ForceGarbageCollection();

	if (const APlayerTeleporter* Teleporter = ThisData->Teleporter.LoadSynchronous())
	{
		Teleporter->TeleportPlayer();
	}

	PlayerChar->SetPlayerSettings(ThisData->PlayerSettings);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this,
		&UGameSectionManager::FinishTransition, 1.0f, false);
}

void UGameSectionManager::FinishTransition()
{
	HideLoadingWidget([this]()
	{
		UInventoryComponent* Inventory = PlayerChar->GetGameMode()->Inventory;
		for (const FInventorySlotData& Item : ThisData->Inventory)
		{
			if (!Item.IsValidSlot()) continue;
			const UInventoryItemData* ItemData = Item.GetItemData<UInventoryItemData>();
			
			FGuid Slot = Inventory->FindSlot(ItemData, {}); 
			if ((!Slot.IsValid() || ItemData->StackingMode == EInventoryItemStackType::Unique)
				&& ItemData->ItemType != EInventoryItemType::Equipment)
			{
				Inventory->AddItem(ItemData, Item.Amount, Item.Metadata, true);
			}
			else if (Slot.IsValid())
			{
				FInventorySlotData* SlotData = Inventory->ItemSlots.Find(Slot);
				SlotData->Amount = FMath::Min(FMath::Max(SlotData->Amount, Item.Amount), ItemData->GetStackLimit());
				SlotData->Metadata.Append(Item.Metadata);
			}
		}
		
		Inventory->ImportSaveData(SaveSystem->GetGameDataObject()->Inventory);

		PlayerChar->FadeFromBlack(1.0f);
		PlayerChar->ClearLockFlag(Player::LockFlags::Loading);
		PlayerChar->GetGameState()->SetGameMusic(ThisData->MusicID);
		PlayerChar->GetGameMode()->Narrative->BeginQuest(ThisData->Quest.LoadSynchronous());

		GetWorld()->GetLevelScriptActor()->RemoteEvent(TEXT("OnSectionLoaded"));
		
		bLoading = false;	
	});
}

bool UGameSectionManager::UnloadLevel(const TSoftObjectPtr<UWorld>& InMap)
{
	const FName MapName(*FPackageName::ObjectPathToPackageName(InMap.ToString()));
	ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this, MapName);
	if (!Level || !Level->IsLevelLoaded()) return false;

	// Let's not unload something only to load it again...
	if (ThisData && ThisData->Levels.Contains(InMap))
	{
		Level->SetShouldBeVisible(ThisData->Levels.FindRef(InMap));
		return false;
	}
	
	if (Level->IsLevelVisible() || Level->IsLevelLoaded())
	{
		UnloadingLevels++;
		FLatentActionInfo Info;
		Info.Linkage = 0;
		Info.UUID = GetLatentID();
		Info.CallbackTarget = this;
		Info.ExecutionFunction = TEXT("OnLevelUnloaded");
		UGameplayStatics::UnloadStreamLevel(this, MapName,
			Info, false);
		return true;
	}

	return false;
}

bool UGameSectionManager::LoadLevel(const TPair<TSoftObjectPtr<UWorld>, bool>& InMap)
{
	const FName MapName(*FPackageName::ObjectPathToPackageName(InMap.Key.ToString()));
	ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(this, MapName);
	if (Level && Level->IsLevelLoaded())
	{
		Level->SetShouldBeVisible(InMap.Value);
		return false;
	}
	
	LoadingLevels++;
	FLatentActionInfo Info;
	Info.Linkage = 0;
	Info.UUID = GetLatentID();
	Info.CallbackTarget = this;
	Info.ExecutionFunction = TEXT("OnLevelLoaded");
	UGameplayStatics::LoadStreamLevel(this, MapName,
		InMap.Value, false, Info);
	return true;
}

void UGameSectionManager::ShowLoadingWidget()
{
	if (!ThisData) return;
	if (ULoadingWidgetBase* Widget = GetLoadingWidget())
	{
		Widget->SetMinimalMode(ThisData->bMinimalLoadingScreen);
		Widget->BeginLoading(ThisData->GetDependencies(),
			ThisData->GetBackground(), ThisData->GetTip());
	}
}

void UGameSectionManager::HideLoadingWidget(const TFunction<void()>& OnFinished)
{
	if (ULoadingWidgetBase* Widget = GetLoadingWidget())
	{
		Widget->FinishLoading(OnFinished);
	}
}

void UGameSectionManager::OnLevelUnloaded()
{
	UnloadingLevels--;
	if (UnloadingLevels == 0)
	{
		LoadCurrentData();
	}
}

void UGameSectionManager::OnLevelLoaded()
{
	LoadingLevels--;
	if (LoadingLevels == 0)
	{
		FinishLoading();
	}
}

ULoadingWidgetBase* UGameSectionManager::GetLoadingWidget()
{
	if (IsValid(LoadingWidget)) return LoadingWidget;
	LoadingWidget = PlayerChar->GetGameMode()->GetWidget<ULoadingWidgetBase>();
	return LoadingWidget;
}

void UGameSectionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bLoading) PlayTime += DeltaTime;
}

void UGameSectionManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	SaveSystem = USaveSubsystem::Get(this);
	PlayerChar = FRPlayer(this);
}

void UGameSectionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SectionGraph = FRSettings->GameSectionGraph.LoadSynchronous();
	if (!SectionGraph)
	{
		SMART_LOG(Error, TEXT("Game Section Manager initialized without any valid Section Graph."))
	}
}

bool UGameSectionManager::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && FRSettings->IsGameplayMap(Outer);
}
