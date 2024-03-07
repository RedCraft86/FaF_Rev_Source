// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCTimingGameManager.h"
#include "Algo/RandomShuffle.h"

void FTimingGameStruct::Tick(float DeltaTime)
{
	if (bStopTick) return;
	if (Time >= 0)
	{
		Time -= DeltaTime;
	}
	else if (!bStopTick)
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
	bInGame = false;
	Progress = 0.0f;
	MaxProgress = 0.0f;
	Instances = {};
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
	
	bInGame = true;
	Progress = 0.0f;
	MaxProgress = FMath::Max(10, InMaxProgress);
	Instances.Empty();

	GetWorld()->GetTimerManager().UnPauseTimer(TickTimer);
	SetComponentTickEnabled(true);
}

void UGCTimingGameManager::OnKeySuccess(const FGuid& ID)
{
	SucceededKeys.Add(ID.ToString());
	RemoveInstance(ID);

	Progress += 10.0f;
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
	
	Algo::RandomShuffle(KeyList);

	const FGuid ID(FGuid::NewGuid());
	const FKey Key(KeyList[0]);

	const TSharedPtr<FTimingGameStruct> Struct = MakeShareable(new FTimingGameStruct(ID, Key, Instances.Num() * 2.0f));
	Struct->OnSuccess.AddUObject(this, &UGCTimingGameManager::OnKeyFailed);
	Struct->OnFailed.AddUObject(this, &UGCTimingGameManager::OnKeySuccess);
	Instances.Add(ID.ToString(), Struct);

	OnAdded.Broadcast(ID);
}

void UGCTimingGameManager::RemoveInstance(const FGuid& ID)
{
	Instances.Remove(ID.ToString());
	OnRemoved.Broadcast(ID);
}

void UGCTimingGameManager::ConsistentTick()
{
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
	for (const TPair<FString, TSharedPtr<FTimingGameStruct>>& Pair : Instances)
	{
		if (Pair.Value.IsValid()) Pair.Value->Tick(DeltaTime);
	}

	if (Progress > 0)
	{
		Progress -= FMath::Min(DeltaTime * 100.0f, 2.0f);
	}
	else
	{
		StopGame(true);
	}
}
