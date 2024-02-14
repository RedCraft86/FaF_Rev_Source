// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "EnemyAIBase.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

AEnemyAIBase::AEnemyAIBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);

	GetMesh()->SetRelativeLocation(FVector{0.0f, 0.0f, -90.0f});
	GetMesh()->SetRelativeRotation(FRotator{0.0f, -90.0f, 0.0f});

	LogicComponent = CreateDefaultSubobject<USMStateMachineComponent>("LogicComponent");
}

FVector AEnemyAIBase::GetEyeWorldLocation_Implementation()
{
	if (const UArrowComponent* ArrowComp = GetEyeArrowComponent())
	{
		return ArrowComp->GetComponentLocation();
	}
	
	return GetActorLocation();
}

FVector AEnemyAIBase::GetEyeForwardVector_Implementation()
{
	if (const UArrowComponent* ArrowComp = GetEyeArrowComponent())
	{
		return ArrowComp->GetForwardVector();
	}
	
	return GetActorForwardVector();
}
