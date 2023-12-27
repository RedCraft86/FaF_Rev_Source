// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCSequenceManager.h"
#include "Core/GCSettings.h"
#include "Core/CoreLogging.h"
#include "GCSequenceOrderAsset.h"
#include "SaveSystem/GCSaveManager.h"
#include "SaveSystem/GCSaveObjects.h"
#include "Player/GCPlayerCharacter.h"
#include "Player/GCPlayerController.h"
#include "Framework/GCTeleportTarget.h"
#include "GameMusic/GCGameMusicManager.h"
#include "Inventory/GCInventoryManager.h"
#include "Narrative/GCNarrativeComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "RCLoadUtilsLibrary.h"
#include "Interfaces/GCSequenceInterface.h"

UGCSequenceManager::UGCSequenceManager()
{
	PlayerCharacter = nullptr;
	PlayerController = nullptr;
	
	NextUUID = 0;
	bInProgress = false;
	ThisSequenceID = NAME_None;
	LastSequenceID = NAME_None;
	SubsequenceIndex = 0;
}

UGCSequenceManager* UGCSequenceManager::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	return World ? World->GetSubsystem<UGCSequenceManager>() : nullptr;
}

void UGCSequenceManager::LoadGameSequence()
{
	if (const UGCSaveManager* SaveManager = UGCSaveManager::Get(this))
	{
		SetGameSequence(NameToGameSequence(SaveManager->GetGameSaveObject()->GameSequenceID));
	}
}

void UGCSequenceManager::SetGameSequence(const FGCSequenceID InSequenceID)
{
	if (ThisSequenceID == *InSequenceID)
	{
		return;
	}
	
	if (bInProgress)
	{
		UE_LOG(GameSequence, Warning, TEXT("Failed to set game sequence to '%s'"), *InSequenceID.ToString());
		UE_LOG(GameSequence, Warning, TEXT("Currently in process of unloading '%s' and loading '%s'"),
			*LastSequenceID.ToString(), *ThisSequenceID.ToString());
		
		return;
	}

	check(IsValid(PlayerCharacter))
	check(IsValid(PlayerController))
	
	bInProgress = true;
	LastSequenceID = ThisSequenceID;
	ThisSequenceID = *InSequenceID;

	PlayerCharacter->SetCanPause(false);
	PlayerCharacter->SetWorldDevice(nullptr);
	PlayerCharacter->SetActiveState(EGCPlayerActiveState::Loading);
	PlayerController->PlayerCameraManager->SetManualCameraFade(1.0f, FLinearColor::Black, true);
	
	PlayerCharacter->ClearEnemies();
	PlayerCharacter->DisableInput(nullptr);
	PlayerCharacter->TeleportTo(FVector::ZeroVector, FRotator::ZeroRotator);
	PlayerCharacter->ResetPlayer();

	OnSequenceChangeStart.Broadcast(ThisSequenceID);
	BeginUnloadWorld();

	if (InSequenceID.IsValid())
	{
		if (const UGCSaveManager* SaveManager = UGCSaveManager::Get(this))
		{
			SaveManager->GetGameSaveObject()->GameSequenceID = ThisSequenceID;
			SaveManager->SaveGameData(nullptr);

			SubsequenceIndex = SaveManager->GetGameSaveObject()->SubsequenceIndexes.FindOrAdd(ThisSequenceID, 0);
		}
	}
	else
	{
		SubsequenceIndex = 0;
	}
}

void UGCSequenceManager::SetSubsequenceIndex(const uint8 Index)
{
	SubsequenceIndex = Index;
	if (!ThisSequenceID.IsNone())
	{
		if (const UGCSaveManager* SaveManager = UGCSaveManager::Get(this))
		{
			SaveManager->GetGameSaveObject()->SubsequenceIndexes.Add(ThisSequenceID, Index);
			SaveManager->SaveGameData(nullptr);
		}
	}
}

void UGCSequenceManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	PlayerCharacter = AGCPlayerCharacter::Get(this);
	PlayerController = AGCPlayerController::Get(this);
}

bool UGCSequenceManager::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE ||
		WorldType == EWorldType::GamePreview || WorldType == EWorldType::GameRPC;
}

void UGCSequenceManager::BeginUnloadWorld()
{
	if (UGCGameMusicManager* MusicManager = UGCGameMusicManager::Get(this))
	{
		UE_LOG(GameSequence, Log, TEXT("Stopping world music..."));
		MusicManager->StopAll();
	}
	
	const FGCSequenceData Data = GetGameSequenceData(LastSequenceID);
	if (Data.IsValidData())
	{
		UGCInventoryManager* Inventory = UGCInventoryManager::Get(this);
		if (Data.InventoryKey.IsValid() && IsValid(Inventory))
		{
			UE_LOG(GameSequence, Log, TEXT("Saving inventory"));
		
			const FGCInvSaveData InvData = Inventory->SaveData();
			if (const UGCSaveManager* SaveManager = UGCSaveManager::Get(this))
			{
				SaveManager->GetGameSaveObject()->InventoryData.Add(Data.InventoryKey, InvData);
			}
		}
		
		if (UGCNarrativeComponent* Narrative = UGCNarrativeComponent::Get(this))
		{
			UE_LOG(GameSequence, Log, TEXT("Forgetting quest '%s'"), *Data.QuestClass.GetAssetName());
			Narrative->ForgetQuest(Data.QuestClass.LoadSynchronous());
		}

		ULevelStreaming* LastLevel = UGameplayStatics::GetStreamingLevel(this, Data.GetLevelName());
		if (LastLevel && LastLevel->IsLevelVisible())
		{
			UE_LOG(GameSequence, Log, TEXT("Calling unload event for level '%s'"), *Data.Level.GetAssetName());
			GCSequence::OnUnloadWorld(LastLevel->GetLevelScriptActor());
		
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, [this, Data]()
			{
				FLatentActionInfo ActionInfo;
				ActionInfo.CallbackTarget = this;
				ActionInfo.ExecutionFunction = FName("OnWorldUnloaded");
				ActionInfo.UUID = GetNextUUID();
				ActionInfo.Linkage = 0;

				UE_LOG(GameSequence, Log, TEXT("Unloading level '%s'"), *Data.Level.GetAssetName());
				UGameplayStatics::UnloadStreamLevel(this, Data.GetLevelName(),
					ActionInfo, false);
			}, 0.5f, false);

			return;
		}
	}
	
	OnWorldUnloaded();
}

void UGCSequenceManager::BeginLoadWorld()
{
	const FGCSequenceData Data = GetGameSequenceData(ThisSequenceID);
	if (Data.IsValidData())
	{
		FLatentActionInfo ActionInfo;
		ActionInfo.CallbackTarget = this;
		ActionInfo.ExecutionFunction = FName("OnWorldLoaded");
		ActionInfo.UUID = GetNextUUID();
		ActionInfo.Linkage = 0;

		UE_LOG(GameSequence, Log, TEXT("Loading level '%s'"), *Data.Level.GetAssetName());
		UGameplayStatics::LoadStreamLevel(this, Data.GetLevelName(),
			true, false, ActionInfo);
	}
	else
	{
		OnWorldLoaded();
	}
}

void UGCSequenceManager::OnWorldUnloaded()
{
	const FGCSequenceData Data = GetGameSequenceData(LastSequenceID);
	if (Data.IsValidData())
	{
		UE_LOG(GameSequence, Log, TEXT("Unloaded level '%s'"), *Data.Level.GetAssetName());
	}

	PlayerController->PlayerCameraManager->SetManualCameraFade(1.0f, FLinearColor::Black, true);
	URCLoadUtilsLibrary::ForceGarbageCollection();
	BeginLoadWorld();
}

void UGCSequenceManager::OnWorldLoaded()
{
	URCLoadUtilsLibrary::ForceGarbageCollection();
	const FGCSequenceData Data = GetGameSequenceData(ThisSequenceID);
	if (Data.IsValidData())
	{
		UE_LOG(GameSequence, Log, TEXT("Loaded level '%s'"), *Data.Level.GetAssetName());
		ULevelStreaming* ThisLevel = UGameplayStatics::GetStreamingLevel(this, Data.GetLevelName());
		if (ThisLevel && ThisLevel->IsLevelLoaded())
		{
			UE_LOG(GameSequence, Log, TEXT("Calling load event for level '%s'"), *Data.Level.GetAssetName());
			GCSequence::OnLoadWorld(ThisLevel->GetLevelScriptActor());
		}

		if (UGCGameMusicManager* MusicManager = UGCGameMusicManager::Get(this))
		{
			UE_LOG(GameSequence, Log, TEXT("Setting main music to '%s'"), *Data.MainMusic.GetAssetName());
			UE_LOG(GameSequence, Log, TEXT("Setting chase music to '%s'"), *Data.ChaseMusic.GetAssetName());
			MusicManager->SetMainMusic(Data.MainMusic.LoadSynchronous());
			MusicManager->SetChaseMusic(Data.ChaseMusic.LoadSynchronous());
		}

		if (UGCNarrativeComponent* Narrative = UGCNarrativeComponent::Get(this))
		{
			UE_LOG(GameSequence, Log, TEXT("Starting quest '%s'"), *Data.QuestClass.GetAssetName());
			Narrative->BeginQuest(Data.QuestClass.LoadSynchronous());
		}

		if (UGCInventoryManager* Inventory = UGCInventoryManager::Get(this))
		{
			UE_LOG(GameSequence, Log, TEXT("Loading inventory"));
		
			FGCInvSaveData OutData = {};
			const UGCSaveManager* SaveManager = UGCSaveManager::Get(this);
			if (Data.InventoryKey.IsValid() && IsValid(SaveManager))
			{
				OutData = SaveManager->GetGameSaveObject()->InventoryData.FindRef(Data.InventoryKey);
			}
			
			Inventory->LoadData(OutData);
			
			for (const FGCInvItemID& ItemID : Data.EnsureItems)
			{
				UE_LOG(GameSequence, Log, TEXT("Ensuring item '%s'"), *ItemID.ToString());
				Inventory->EnsureItem(ItemID);
			}
		}

		if (const AGCTeleportTarget* Teleporter = AGCTeleportTarget::Get(this, Data.TeleporterTag))
		{
			UE_LOG(GameSequence, Log, TEXT("Teleporting to '%s'"), *Teleporter->GetName());
			Teleporter->TeleportPlayer();
		}

		PlayerCharacter->SetAbilityFlags(Data.AbilityFlags);

		for (const TPair<EGCCustomBoolKeys, bool>& CustomBool : Data.CustomBooleanData)
		{
			PlayerCharacter->SetCustomBooleanValue(CustomBool.Key, CustomBool.Value);
		}
		
		for (const TPair<EGCCustomFloatKeys, float>& CustomNumber : Data.CustomNumberData)
		{
			PlayerCharacter->SetCustomNumberValue(CustomNumber.Key, CustomNumber.Value);
		}
	}

	PlayerController->PlayerCameraManager->SetManualCameraFade(1.0f, FLinearColor::Black, true);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [this](){
		PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 1.0f,
			FLinearColor::Black, true, false);

		PlayerCharacter->SetActiveState(EGCPlayerActiveState::Normal);
		PlayerCharacter->SetCanPause(true);
		//PlayerCharacter->EnableInput(nullptr);
	}, 1.0f, false);

	bInProgress = false;
	OnSequenceChangeFinish.Broadcast(ThisSequenceID);
}

bool UGCSequenceManager::IsValidGameSequenceID(const FGCSequenceID InSequenceID)
{
	if (!InSequenceID.IsValid())
	{
		return false;
	}
	
	if (const UDataTable* Table = UGCSettings::Get()->GameSequenceTable.LoadSynchronous())
	{
		return Table->GetRowMap().Contains(*InSequenceID);
	}

	return false;
}

FGCSequenceData UGCSequenceManager::GetGameSequenceData(const FGCSequenceID InSequenceID)
{
	if (!InSequenceID.IsValid())
	{
		return {};
	}
	
	if (const UDataTable* Table = UGCSettings::Get()->GameSequenceTable.LoadSynchronous())
	{
		if (const FGCSequenceData* DataPtr = Table->FindRow<FGCSequenceData>(*InSequenceID, StaticClass()->GetName()))
		{
			return *DataPtr;
		}
	}

	return {};
}

bool UGCSequenceManager::IsValidGameSequenceData(const FGCSequenceData& InSequenceData)
{
	return InSequenceData.IsValidData();
}

FGCSequenceID UGCSequenceManager::GetLastGameSequenceID(const FGCSequenceID InSequenceID)
{
	if (!InSequenceID.IsValid())
	{
		return {};
	}
	
	if (const UGCSequenceOrderAsset* OrderAsset = UGCSettings::Get()->GameSequenceOrder.LoadSynchronous())
	{
		return OrderAsset->GetLastID(InSequenceID);
	}

	return {};
}

FGCSequenceID UGCSequenceManager::GetNextGameSequenceID(const FGCSequenceID InSequenceID)
{
	if (!InSequenceID.IsValid())
	{
		return {};
	}
	
	if (const UGCSequenceOrderAsset* OrderAsset = UGCSettings::Get()->GameSequenceOrder.LoadSynchronous())
	{
		return OrderAsset->GetNextID(InSequenceID);
	}

	return {};
}

int32 UGCSequenceManager::GetGameSequenceAsPercent(const FGCSequenceID InSequenceID)
{
	if (!InSequenceID.IsValid())
	{
		return 0.0f;
	}
	
	if (const UGCSequenceOrderAsset* OrderAsset = UGCSettings::Get()->GameSequenceOrder.LoadSynchronous())
	{
		return OrderAsset->GetPercentByID(InSequenceID);
	}

	return 0.0f;
}

FGCSequenceID UGCSequenceManager::NameToGameSequence(const FName InName)
{
	const FGCSequenceID PossibleID(InName);
	if (IsValidGameSequenceID(PossibleID))
	{
		return PossibleID;
	}

	const UGCSequenceOrderAsset* OrderAsset = UGCSettings::Get()->GameSequenceOrder.LoadSynchronous();
	if (OrderAsset && !OrderAsset->OrderedSequences.IsEmpty())
	{
		return OrderAsset->OrderedSequences[0];
	}

	return {};
}
