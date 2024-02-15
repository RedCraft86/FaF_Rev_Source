// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "EnemyAIBase.h"
#include "PlayerSensingComponent.h"
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

	SensingComponent = CreateDefaultSubobject<UPlayerSensingComponent>("SensingComponent");
	SensingComponent->SetRelativeRotation(FRotator{0.0f, -90.0f, 0.0f});
	SensingComponent->SetRelativeScale3D(FVector{0.5f});
	SensingComponent->SetupAttachment(GetMesh());
	SensingComponent->bEnabled = false;
	
	LogicComponent = CreateDefaultSubobject<USMStateMachineComponent>("LogicComponent");
	LogicComponent->bStartOnBeginPlay = false;

	bStartEnabled = true;
	bEnabled = false;
}

void AEnemyAIBase::GetRequirements_Implementation(bool& Sensing, bool& StateMachine, bool& Ticking) const
{
	Sensing = true;
	StateMachine = true;
	Ticking = true;
}

void AEnemyAIBase::SetEnabled(const bool bInEnabled)
{
	if (bEnabled != bInEnabled)
	{
		bEnabled = bInEnabled;

		bool Sensing, StateMachine, Ticking = false;
		GetRequirements(Sensing, StateMachine, Ticking);
		SetActorTickEnabled(Ticking ? bEnabled : false);
		
		SensingComponent->SetEnabled(Sensing ? bEnabled : false);
		if (!StateMachine || !bEnabled)
		{
			LogicComponent->Stop();
		}
		else
		{
			LogicComponent->Start();
		}
	}
}

FVector AEnemyAIBase::GetEyeWorldLocation_Implementation()
{
	return SensingComponent->GetComponentLocation();
}

FVector AEnemyAIBase::GetEyeForwardVector_Implementation()
{
	return SensingComponent->GetForwardVector();
}

void AEnemyAIBase::BeginPlay()
{
	Super::BeginPlay();
	SetEnabled(bStartEnabled);
}

void AEnemyAIBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SensingComponent->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform, GetEyeAttachBone());
}
