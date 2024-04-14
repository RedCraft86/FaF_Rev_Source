// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSectionManager.h"
#include "Kismet/GameplayStatics.h"
#include "UserWidgets/LoadingWidget.h"
#include "GameSection/Graph/GameSectionNode.h"
#include "Libraries/GTLoadUtilsLibrary.h"
#include "FRSettings.h"
#include "FaF_Rev.h"


void UGameSectionManager::Step(const int32 Index)
{
	if (!SectionGraph)
	{
		SMART_LOG(Error, TEXT("Attempting to Step with a null Section Graph."))
		return;
	}
	
	if (IsBusy())
	{
		SMART_LOG(Warning, TEXT("Game Section Manager is busy, the Step will be ignored."));
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
	LastData = ThisData;
	const UGameSectionNode* Node = SectionGraph->GetNodeBySequence<UGameSectionNode>(Sequence);
	if (!Node || !Node->Sequence || !Node->Sequence->IsA(UGameSectionData::StaticClass()))
	{
		SMART_LOG(Error, TEXT("Cannot begin transition. Node or sequence is null."))
		return;
	}
	ThisData = Cast<UGameSectionData>(Node->Sequence);
	
	FTimerHandle Handle;
	if (LastData)
	{
		
		GetWorld()->GetTimerManager().SetTimer(Handle, this,
			&UGameSectionManager::UnloadLastData, 0.1f, false);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(Handle, this,
			&UGameSectionManager::LoadCurrentData, 0.1f, false);
	}
}

void UGameSectionManager::UnloadLastData()
{
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

	if (LoadingWidget) LoadingWidget->bUnloading = false;

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

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this,
		&UGameSectionManager::FinishTransition, 1.0f, false);
}

void UGameSectionManager::FinishTransition()
{
	HideLoadingWidget([this]()
	{
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

void UGameSectionManager::HideLoadingWidget(const TFunction<void()>& OnFinished) const
{
	if (LoadingWidget)
	{
		LoadingWidget->FinishLoading(OnFinished);
	}
}

void UGameSectionManager::ShowLoadingWidget() const
{
	if (LoadingWidget && ThisData)
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

void UGameSectionManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (UClass* WidgetClass = FRSettings->LoadingWidgetClass.LoadSynchronous())
	{
		LoadingWidget = CreateWidget<ULoadingWidgetBase>(InWorld.GetFirstPlayerController(), WidgetClass);	
	}
	
	if (!LoadingWidget)
	{
		SMART_LOG(Error, TEXT("Game Section Manager doesn't have a Loading Widget to show."))
	}
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
	return bSuper && FRSettings->GameplayMap.GetAssetName() == Outer->GetOutermostObject()->GetName();
}
