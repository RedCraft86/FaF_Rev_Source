﻿// Copyright (C) RedCraft86. All Rights Reserved.

#include "FREnemyBase.h"
#include "FRPlayer.h"
#include "SMStateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AFREnemyBase::AFREnemyBase() : bStartRoaming(true), EnemyState(EEnemyState::None)
{
	LogicComponent = CreateDefaultSubobject<USMStateMachineComponent>("LogicComponent");
}

void AFREnemyBase::DisableAI()
{
	LogicComponent->Stop();
	GetCharacterMovement()->StopMovementImmediately();
}

void AFREnemyBase::SetEnemyState(const EEnemyState InNewState)
{
	if (EnemyState != InNewState)
	{
		EnemyState = InNewState;
		if (AFRPlayerBase* Player = PlayerChar.LoadSynchronous())
		{
			if (EnemyState == EEnemyState::None || EnemyState == EEnemyState::Roam)
			{
				Player->RemoveEnemy(this);
			}
			else
			{
				Player->AddEnemy(this);
			}
		}
	}
}

void AFREnemyBase::BeginPlay()
{
	Super::BeginPlay();
	SetEnemyState(bStartRoaming ? EEnemyState::Roam : EEnemyState::None);
	if (EnemyState != EEnemyState::None) LogicComponent->Start();
}

EEnemyState AFREnemyBase::GetHighestEnemyState(const TArray<AFREnemyBase*>& InEnemies)
{
	for (const AFREnemyBase* Enemy : InEnemies)
	{
		if (!Enemy) continue;
		switch (Enemy->GetEnemyState())
		{
		case EEnemyState::Chase: return EEnemyState::Chase;
		case EEnemyState::Search: return EEnemyState::Search;
		case EEnemyState::Alert: return EEnemyState::Alert;
		case EEnemyState::Roam: return EEnemyState::Roam;
		case EEnemyState::None: return EEnemyState::None;
		}
	}

	return EEnemyState::None;
}
