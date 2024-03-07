// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCTimingGameManager.h"
#include "Algo/RandomShuffle.h"

void FTimingGameStruct::Tick(const float DeltaTime)
{
	if (bStopTick) return;
	Time -= DeltaTime;
	
	if (Time < 0)
	{
		bStopTick = true;
		OnFailed.Broadcast(ID);
	}
}

void FTimingGameStruct::MarkCompleted()
{
	bStopTick = true;
	OnSuccess.Broadcast(ID);
}

void FTimingGameStruct::MarkFailed()
{
	if (!bStopTick)
	{
		Time = 0.01f;
	}
}

UGCTimingGameManager::UGCTimingGameManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	Multipliers = FVector2D{10.0f};
	MovePerPhase = 10;
	bInGame = false;
	NumMoves = 0;
	Phase = 0;
	Progress = 0.0f;
	MaxProgress = 0.0f;
	Instances = {};
}

float UGCTimingGameManager::GetProgressFromID(const FGuid& InID) const
{
	const TSharedPtr<FTimingGameStruct> Found = Instances.FindRef(InID.ToString());
	if (Found.IsValid()) return Found->Time / Found->MaxTime;

	return 0.0f;
}

FKey UGCTimingGameManager::GetKeyFromID(const FGuid& InID) const
{
	const TSharedPtr<FTimingGameStruct> Found = Instances.FindRef(InID.ToString());
	if (Found.IsValid()) return Found->Key;

	return FKey{};
}

void UGCTimingGameManager::RegisterKeyPress(const FKey& InKey)
{
	for (const TPair<FString, TSharedPtr<FTimingGameStruct>>& Pair : Instances)
	{
		if (Pair.Value.IsValid() && Pair.Value->Key == InKey)
		{
			Pair.Value->MarkCompleted();
			return;
		}
	}

	TArray<FString> Keys;
	Instances.GenerateKeyArray(Keys);
	if (!Keys.IsEmpty())
	{
		Algo::RandomShuffle(Keys);
		const TSharedPtr<FTimingGameStruct> Found = Instances.FindRef(Keys[0]);
		if (Found.IsValid()) Found->MarkFailed();
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

void UGCTimingGameManager::OnKeySuccess(const FGuid& ID)
{
	SucceededKeys.Add(ID.ToString());
	RemoveInstance(ID);

	Progress += Multipliers.X;
	if (Progress > MaxProgress)
	{
		StopGame(false);
	}
}

void UGCTimingGameManager::OnKeyFailed(const FGuid& ID)
{
	FailedKeys.Add(ID.ToString());
	RemoveInstance(ID);
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

	const FGuid ID(FGuid::NewGuid());
	const FKey Key(KeyList[0]);
	
	const TSharedPtr<FTimingGameStruct> Struct = MakeShareable(
		new FTimingGameStruct(ID, Key, 5.0f + Instances.Num() * 2.0f));
	
	Struct->OnSuccess.AddUObject(this, &UGCTimingGameManager::OnKeySuccess);
	Struct->OnFailed.AddUObject(this, &UGCTimingGameManager::OnKeyFailed);
	Instances.Add(ID.ToString(), Struct);

	OnAdded.Broadcast(ID);
}

void UGCTimingGameManager::RemoveInstance(const FGuid& ID)
{
	Instances.Remove(ID.ToString());
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

	TArray<TSharedPtr<FTimingGameStruct>> Values;
	Instances.GenerateValueArray(Values);
	for (const TSharedPtr<FTimingGameStruct>& Value : Values)
	{
		if (Value.IsValid()) Value->Tick(DeltaTime);
	}

	if (Progress > 0)
	{
		Progress -= FMath::Min(DeltaTime * Multipliers.Y, 2.0f);
	}
	else
	{
		StopGame(true);
	}
}
