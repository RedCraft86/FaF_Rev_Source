// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSectionManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameSection/Graph/GameSectionNode.h"
#include "Libraries/GTLoadUtilsLibrary.h"
#include "FRSettings.h"
#include "FaF_Rev.h"

void UGameSectionManager::Step(const int32 Index)
{
	if (!SectionGraph)
	{
		SMART_LOG(Error, TEXT("Attempting to Step with a null SectionGraph."))
		return;
	}

	if (bLoading)
	{
		SMART_LOG(Error, TEXT("Attempting to Step while GameSectionManager is busy."))
		return;
	}

	const TArray<int32> PreUpdate = Sequence;
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
	LastData = ThisData;
	const UGameSectionNode* Node = SectionGraph->GetNodeBySequence<UGameSectionNode>(Sequence);
	if (!Node || !Node->Sequence || !Node->Sequence->IsA(UGameSectionData::StaticClass()))
	{
		SMART_LOG(Error, TEXT("Cannot begin transition. Node or sequence is null."))
		return;
	}
	ThisData = Cast<UGameSectionData>(Node->Sequence);
	
	LastData ? UnloadLastData() : LoadCurrentData();
}

void UGameSectionManager::UnloadLastData()
{
	UnloadLevels = 0;
	LoadedObjs.Empty(ThisData ? ThisData->Backgrounds.Num() + ThisData->PreloadObjects.Num() : 0);
	
	bool bHasLevels = false;
	if (LastData)
	{
		for (const TPair<TSoftObjectPtr<UWorld>, bool>& Pair : LastData->Levels)
		{
			if (UnloadLevel(Pair.Key)) bHasLevels = true;
		}
	}

	if (!bHasLevels)
	{
		LoadCurrentData();
	}
}

void UGameSectionManager::LoadCurrentData()
{
	UGTLoadUtilsLibrary::ForceGarbageCollection();
	
	LoadLevels = 0;
	bool bHasLevels = false;
	if (ThisData)
	{
		for (const TPair<TSoftObjectPtr<UWorld>, bool>& Pair : ThisData->Levels)
		{
			if (LoadLevel(Pair.Key)) bHasLevels = true;
		}

		FStreamableDelegate Callback;
		Callback.BindUObject(this, &UGameSectionManager::OnObjectLoaded);
		FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();
		for (const TSoftObjectPtr<UTexture2D>& Obj : ThisData->Backgrounds)
		{
			StreamObjs.AddUnique(StreamManager.RequestAsyncLoad(
				Obj.ToSoftObjectPath(), Callback));
		}
		for (const TSoftObjectPtr<UObject>& Obj : ThisData->PreloadObjects)
		{
			StreamObjs.AddUnique(StreamManager.RequestAsyncLoad(
				Obj.ToSoftObjectPath(), Callback));
		}
	}

	if (!bHasLevels)
	{
		FinishLoading();
	}
}

void UGameSectionManager::FinishLoading()
{
	UGTLoadUtilsLibrary::ForceGarbageCollection();

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this,
		&UGameSectionManager::FinishTransition, 1.0f, false);
}

void UGameSectionManager::FinishTransition()
{
	bLoading = false;
}

bool UGameSectionManager::UnloadLevel(const TSoftObjectPtr<UWorld>& Level)
{
	// Let's not unload something only to load it again right after...
	if (ThisData && ThisData->Levels.Contains(Level))
	{
		return false;
	}
	
	const FName MapName(Level.GetLongPackageName());
	const ULevelStreaming* Lvl = UGameplayStatics::GetStreamingLevel(this, MapName);
	if (Lvl && (Lvl->IsLevelVisible() || Lvl->IsLevelLoaded()))
	{
		UnloadLevels++;
		FLatentActionInfo Info;
		Info.Linkage = 0;
		Info.UUID = GetNextUUID();
		Info.CallbackTarget = this;
		Info.ExecutionFunction = TEXT("OnLevelUnloaded");
		UGameplayStatics::UnloadStreamLevel(this, MapName,
			Info, false);
		return true;
	}

	return false;
}

bool UGameSectionManager::LoadLevel(const TSoftObjectPtr<UWorld>& Level)
{
	const FName MapName(Level.GetLongPackageName());
	const ULevelStreaming* Lvl = UGameplayStatics::GetStreamingLevel(this, MapName);
	if (Lvl ? !Lvl->IsLevelVisible() || !Lvl->IsLevelLoaded() : true)
	{
		LoadLevels++;
		FLatentActionInfo Info;
		Info.Linkage = 0;
		Info.UUID = GetNextUUID();
		Info.CallbackTarget = this;
		Info.ExecutionFunction = TEXT("OnLevelLoaded");
		UGameplayStatics::LoadStreamLevel(this, MapName,
			true, false, Info);
		return true;
	}

	return false;
}

void UGameSectionManager::OnObjectLoaded()
{
	for (int i = StreamObjs.Num() - 1; i >= 0; i--)
	{
		if (StreamObjs[i]->HasLoadCompleted())
		{
			TArray<UObject*> Assets;
			StreamObjs[i]->GetLoadedAssets(Assets);
			LoadedObjs.Append(Assets);
		}
	}
}

void UGameSectionManager::OnLevelUnloaded()
{
	UnloadLevels--;
	if (UnloadLevels < 1)
	{
		LoadCurrentData();
	}
}

void UGameSectionManager::OnLevelLoaded()
{
	LoadLevels--;
	if (LoadLevels < 1)
	{
		FinishLoading();
	}
}

void UGameSectionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SectionGraph = UFRSettings::Get()->GameSectionGraph.LoadSynchronous();
}
