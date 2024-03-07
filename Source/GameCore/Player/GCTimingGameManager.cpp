// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCTimingGameManager.h"
#include "Algo/RandomShuffle.h"

UGCTimingGameManager::UGCTimingGameManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	BumpSpeed = FVector2D{20.0f, 10.0f};
	DrainSpeed = 15.0f;
	MovePerPhase = 10;
	bInGame = false;
	NumMoves = 0;
	Phase = 0;
	Progress = 0.0f;
	MaxProgress = 0.0f;
	Instances = {};
}

FKey UGCTimingGameManager::GetKeyFromID(const FString& InID) const
{
	return Instances.FindRef(InID);
}

void UGCTimingGameManager::RegisterKeyPress(const FKey& InKey)
{
	TArray<FString> Keys;
	Instances.GenerateKeyArray(Keys);
	
	for (const FString& Key : Keys)
	{
		if (Instances.FindRef(Key) == InKey)
		{
			OnKeySuccess(Key);
			return;
		}
	}
	
	if (!Keys.IsEmpty())
	{
		Algo::RandomShuffle(Keys);
		OnKeyFailed(Keys[0]);
	}
}

void UGCTimingGameManager::BeginNewGame(const float InMaxProgress)
{
	if (bInGame) return;
	
	FailedKeys.Empty();
	SucceededKeys.Empty();

	NumMoves = 0;
	Phase = 0;
	bInGame = true;
	MaxProgress = FMath::Max(10, InMaxProgress);
	Progress = MaxProgress * 0.5f;
	Instances.Empty();

	GetWorld()->GetTimerManager().UnPauseTimer(TickTimer);
	SetComponentTickEnabled(true);

	OnStarted.Broadcast();
}

void UGCTimingGameManager::OnKeySuccess(const FString& ID)
{
	SucceededKeys.Add(ID);
	RemoveInstance(ID);

	Progress += BumpSpeed.X;
	if (Progress > MaxProgress)
	{
		StopGame(false);
	}
}

void UGCTimingGameManager::OnKeyFailed(const FString& ID)
{
	FailedKeys.Add(ID);
	RemoveInstance(ID);

	Progress -= BumpSpeed.Y; 
}

void UGCTimingGameManager::CreateInstance()
{
	if (KeyList.IsEmpty()) return;

	uint8 Attempts = 0;
	const FKey LastKey(KeyList[0]);
	Algo::RandomShuffle(KeyList);
	while (LastKey == KeyList[0] && Attempts < 5)
	{
		Algo::RandomShuffle(KeyList);
		Attempts++;
	}

	const FString ID(FGuid::NewGuid().ToString());
	Instances.Add(ID, KeyList[0]);
	OnAdded.Broadcast(ID);
}

void UGCTimingGameManager::RemoveInstance(const FString& ID)
{
	Instances.Remove(ID);
	OnRemoved.Broadcast(ID);
	NumMoves++;
}

void UGCTimingGameManager::ConsistentTick()
{
	if (NumMoves > MovePerPhase)
	{
		Phase++;
		NumMoves = 0;
	}
	
	switch (Phase)
	{
	case 0:
		if (Instances.IsEmpty())
		{
			CreateInstance();
		}
		break;
		
	case 1:
		if (Instances.Num() < 2)
		{
			CreateInstance();
		}
		break;
		
	case 2:
		if (Instances.Num() < 4)
		{
			CreateInstance();
		}
		break;
		
	default:
		if (Instances.Num() < 6)
		{
			CreateInstance();
		}
		break;
	}
}

void UGCTimingGameManager::StopGame(const bool bFailed)
{
	GetWorld()->GetTimerManager().PauseTimer(TickTimer);
	SetComponentTickEnabled(false);

	NumMoves = 0;
	Phase = 0;
	Progress = 0.0f;
	Instances.Empty();
	FailedKeys.Empty();
	SucceededKeys.Empty();
	bInGame = false;
	
	bFailed ? OnFailed.Broadcast() : OnSuccess.Broadcast();
}

void UGCTimingGameManager::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TickTimer, this, &ThisClass::ConsistentTick, 0.25f, true);
	GetWorld()->GetTimerManager().PauseTimer(TickTimer);
}

void UGCTimingGameManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Progress > 0)
	{
		Progress -= FMath::Min(DeltaTime * DrainSpeed, 2.0f);
	}
	else
	{
		StopGame(true);
	}
}
