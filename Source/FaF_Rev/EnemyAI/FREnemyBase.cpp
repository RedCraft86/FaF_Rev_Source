// Copyright (C) RedCraft86. All Rights Reserved.

#include "FREnemyBase.h"

AFREnemyBase::AFREnemyBase() : bStartRoaming(true), EnemyState(EEnemyState::None)
{
	PrimaryActorTick.bCanEverTick = true;
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
}

void AFREnemyBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (PlayerChar.IsNull()) PlayerChar = AFRPlayerBase::Get(this);
}
