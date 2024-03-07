// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCQuickTimeEventManager.h"
#include "UserInterface/GCQuickTimeEventWidget.h"
#include "GCPlayerController.h"
#include "Algo/RandomShuffle.h"

UGCQuickTimeEventManager::UGCQuickTimeEventManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	WidgetClass = NULL;
	Keylist = {EKeys::W, EKeys::S, EKeys::A, EKeys::D};
	Speeds = FVector(15.0f, 20.0f, 10.0f);
	MovesPerPhase = 5;
	WidgetObject = nullptr;
	ResetData();
}

void UGCQuickTimeEventManager::BeginNewQTE(const float InMaxProgress)
{
	if (bPlaying || Keylist.IsEmpty()) return;
	
	ResetData();
	bPlaying = true;
	MaxProgress = FMath::Max(10.0f, InMaxProgress);
	Progress = MaxProgress * 0.5f;
	
	GetWorld()->GetTimerManager().UnPauseTimer(TickTimer);
	OnStarted.Broadcast();

	WidgetObject->AddUserWidget();
}

void UGCQuickTimeEventManager::RegisterKeyPress(const FKey& InKey)
{
	if (!InKey.IsValid()) return;
	
	TArray<FString> IDs;
	Instances.GenerateKeyArray(IDs);
	for (const FString& ID : IDs)
	{
		if (Instances.FindRef(ID) == InKey)
		{
			OnKeySuccess(ID);
			return;
		}
	}

	if (!IDs.IsEmpty())
	{
		Algo::RandomShuffle(IDs);
		OnKeyFailed(IDs[0]);
	}
}

void UGCQuickTimeEventManager::CreateInstance()
{
	uint8 Attempts = 0;
	const FKey Last(Keylist[0]);
	Algo::RandomShuffle(Keylist);
	while (Keylist[0] == Last && Attempts < 5)
	{
		Algo::RandomShuffle(Keylist);
		Attempts++;
	}

	const FString ID(FGuid::NewGuid().ToString());
	Instances.Add(ID, Keylist[0]);
	OnKeyAdded.Broadcast(ID);
}

void UGCQuickTimeEventManager::RemoveInstance(const FString& InID)
{
	if (Instances.Remove(InID) > 0)
	{
		OnKeyRemoved.Broadcast(InID);
		NumMoves++;
	}
}

void UGCQuickTimeEventManager::OnKeySuccess(const FString& InID)
{
	SucceededKeys.Add(InID);
	RemoveInstance(InID);
	Progress += Speeds.Y;
	if (Progress > MaxProgress)
	{
		StopQTE(false);
	}
}

void UGCQuickTimeEventManager::OnKeyFailed(const FString& InID)
{
	FailedKeys.Add(InID);
	RemoveInstance(InID);
	Progress -= Speeds.Z;
}

void UGCQuickTimeEventManager::StopQTE(const bool bFailed)
{
	WidgetObject->RemoveUserWidget();
	GetWorld()->GetTimerManager().PauseTimer(TickTimer);
	bFailed ? OnFailed.Broadcast() : OnSuccess.Broadcast();
	ResetData();
}

void UGCQuickTimeEventManager::ResetData()
{
	bPlaying = false;
	Phase = 0;
	NumMoves = 0;
	Progress = 0.0f;
	MaxProgress = 100.0f;
	SucceededKeys = {};
	FailedKeys = {};
	Instances = {};
}

void UGCQuickTimeEventManager::TimedTick()
{
	if (Progress > 0.0f)
	{
		Progress -= Speeds.X;
	}
	else
	{
		StopQTE(true);
	}

	if (NumMoves > MovesPerPhase && Phase < 3)
	{
		Phase++;
		NumMoves = 0;
	}

	if (Instances.Num() < FMath::Max(1, Phase * 2))
	{
		CreateInstance();
	}
}

void UGCQuickTimeEventManager::BeginPlay()
{
	Super::BeginPlay();

	WidgetObject = CreateWidget<UGCQuickTimeEventWidget>(AGCPlayerController::Get(this), WidgetClass);
	WidgetObject->Setup(this);

	GetWorld()->GetTimerManager().SetTimer(TickTimer, this, &ThisClass::TimedTick, 0.05f, true);
	GetWorld()->GetTimerManager().PauseTimer(TickTimer);
}
