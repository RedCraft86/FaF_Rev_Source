// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "EnemyAIBase.h"
#include "PlayerSensing.h"
#include "Components/CapsuleComponent.h"
#include "SMStateMachineComponent.h"

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
	SensingComponent = CreateDefaultSubobject<UPlayerSensing>("SensingComponent");
	SensingComponent->SetupAttachment(GetMesh());
}

FVector AEnemyAIBase::GetEyeWorldLocation_Implementation()
{
	return SensingComponent->GetComponentLocation();
}

FVector AEnemyAIBase::GetEyeForwardVector_Implementation()
{
	return SensingComponent->GetForwardVector();
}

void AEnemyAIBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SensingComponent->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform, GetEyeAttachBone());
}
