// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSectionManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameSection/Graph/GameSectionNode.h"
#include "GameSection/Graph/GameSectionGraph.h"
#include "Libraries/GTLoadUtilsLibrary.h"
#include "PlayerTeleporter.h"
#include "LoadingWidget.h"
#include "FRGameMode.h"
#include "FRSettings.h"
#include "FaF_Rev.h"

void UGameSectionManager::Step(const uint8 Index)
{
	if (!SectionGraph)
	{
		SMART_LOG(Error, TEXT("Attempting to Step with a null Section Graph."));
		return;
	}
	
	if (IsBusy())
	{
		SMART_LOG(Warning, TEXT("Game Section Manager is busy, the Step request will be ignored."));
		return;
	}

	const TArray<uint8> PreUpdate = Sequence;
	Sequence.Add(Sequence.IsEmpty() ? 0 : Index);
	Sequence = SectionGraph->ValidateSequence(Sequence);

	if (PreUpdate != Sequence)
	{
		BeginTransition();
	}
}

void UGameSectionManager::BeginTransition()
{
	bLoading = true;
	if (PlayerChar)
	{
		PlayerChar->AddLockFlag(Player::LockFlags::Loading);
		PlayerChar->FadeToBlack(0.25f);
	}
	
	LastData = ThisData;
	const UGameSectionNode* Node = SectionGraph->GetNodeBySequence<UGameSectionNode>(Sequence);
	if (!Node || !Node->Sequence || !Node->Sequence->IsA(UGameSectionData::StaticClass()))
	{
		SMART_LOG(Error, TEXT("Cannot begin transition. Node or sequence is null."))
		return;
	}
	ThisData = Cast<UGameSectionData>(Node->Sequence);

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
	if (PlayerChar)
	{
		PlayerChar->ResetStates();
		PlayerChar->TeleportPlayer(FVector::ZeroVector, FRotator::ZeroRotator);
		PlayerChar->GetGameMode()->MuteGameMusic();
	}
	
	LoadedObjs.Empty(ThisData ? ThisData->PreloadObjects.Num() : 0);
	UnloadingLevels = 0;
	bool bHasMaps = false;
	if (LastData)
	{
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

	if (ULoadingWidgetBase* LoadingWidget = GetLoadingWidget()) LoadingWidget->bUnloading = false;

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

	if (PlayerChar)
	{
		PlayerChar->SetPlayerSettings(ThisData->PlayerSettings);
	}

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this,
		&UGameSectionManager::FinishTransition, 1.0f, false);
}

void UGameSectionManager::FinishTransition()
{
	HideLoadingWidget([this]()
	{
		if (PlayerChar)
		{
			PlayerChar->FadeFromBlack(1.0f);
			PlayerChar->ClearLockFlag(Player::LockFlags::Loading);
			PlayerChar->GetGameMode()->SetGameMusic(ThisData->MusicID);
		}
		
		bLoading = false;	
	});
}

bool UGameSectionManager::UnloadLevel(const TSoftObjectPtr<UWorld>& InMap)
{
	const FName MapName(InMap.GetLongPackageName());
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
	const FName MapName(InMap.Key.GetLongPackageName());
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

void UGameSectionManager::HideLoadingWidget(const TFunction<void()>& OnFinished)
{
	if (ULoadingWidgetBase* LoadingWidget = GetLoadingWidget())
	{
		LoadingWidget->FinishLoading(OnFinished);
	}
}

void UGameSectionManager::ShowLoadingWidget()
{
	if (!ThisData) return;
	if (ULoadingWidgetBase* LoadingWidget = GetLoadingWidget())
	{
		LoadingWidget->BeginLoading(ThisData->GetDependencies(),
			ThisData->GetBackground(), ThisData->GetTip());
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
	if (IsValid(LoadingWidgetCache)) return LoadingWidgetCache;
	if (AFRGameModeBase* GameMode = FRGamemode(this))
	{
		LoadingWidgetCache = GameMode->GetWidget<ULoadingWidgetBase>();
		return LoadingWidgetCache;
	}

	return nullptr;
}

void UGameSectionManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
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
	const bool bSuper = Super::ShouldCreateSubsystem(Outer);
	return bSuper && FRSettings->OnGameplayMap(Outer);
}
