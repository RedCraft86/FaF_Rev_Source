// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCQuickTimeEventManager.h"
#include "UserInterface/GCQuickTimeEventWidget.h"
#include "GCPlayerController.h"
#include "Algo/RandomShuffle.h"

#define BUMP Speeds.X
#define PENALTY Speeds.Y
#define CONSTANT Speeds.Z

UGCQuickTimeEventManager::UGCQuickTimeEventManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	WidgetClass = NULL;
	Keylist = {EKeys::W, EKeys::S, EKeys::A, EKeys::D};
	Speeds = FVector(20.0f, 10.0f, 10.0f);
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

	for (const FString& ID : InstanceKeys)
	{
		if (Instances.FindRef(ID) == InKey)
		{
			MarkKeySuccess(ID);
			return;
		}
	}

	if (!InstanceKeys.IsEmpty())
	{
		MarkKeyFailed(InstanceKeys[0]);
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
	InstanceKeys.Add(ID);
	OnKeyAdded.Broadcast(ID);
}

void UGCQuickTimeEventManager::RemoveInstance(const FString& InID)
{
	InstanceKeys.Remove(InID);
	if (Instances.Remove(InID) > 0)
	{
		OnKeyRemoved.Broadcast(InID);
		NumMoves++;
	}
}

void UGCQuickTimeEventManager::MarkKeySuccess(const FString& InID)
{
	OnKeySuccess.Broadcast(InID);
	SucceededKeys.Add(InID);
	RemoveInstance(InID);
	Progress += BUMP;
	if (Progress > MaxProgress)
	{
		StopQTE(false);
	}
}

void UGCQuickTimeEventManager::MarkKeyFailed(const FString& InID)
{
	OnKeyFailed.Broadcast(InID);
	FailedKeys.Add(InID);
	RemoveInstance(InID);
	Progress -= PENALTY;
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
	InstanceKeys = {};
}

void UGCQuickTimeEventManager::TimedTick()
{
	if (Progress > 0.0f)
	{
		Progress -= CONSTANT * 0.05f;
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
