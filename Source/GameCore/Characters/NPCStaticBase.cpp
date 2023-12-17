// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "NPCStaticBase.h"
#include "Player/GCPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

ANPCStaticBase::ANPCStaticBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	SetRootComponent(CapsuleComponent);
	
	MeshComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	MeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	MeshComponent->bAffectDynamicIndirectLighting = true;
	MeshComponent->bCastDynamicShadow = true;
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetCollisionProfileName(TEXT("CharacterMesh"));
	MeshComponent->SetupAttachment(CapsuleComponent);
	
	EyePosition = CreateDefaultSubobject<UArrowComponent>("EyePosition");
	EyePosition->SetupAttachment(MeshComponent);

	LookAtComponent = CreateDefaultSubobject<USceneComponent>("LookAtComponent");
	LookAtComponent->SetupAttachment(MeshComponent);
#if WITH_EDITORONLY_DATA
	LookAtComponent->bVisualizeComponent = true;
#endif

	HeadSocketName = NAME_None;
	LookAtLocation = FVector::ZeroVector;
}

void ANPCStaticBase::GetPlayerCameraInfo(float& AngleTo, FVector& Location) const
{
	if (const AGCPlayerCharacter* Player = AGCPlayerCharacter::Get(this))
	{
		Location = Player->PlayerCamera->GetComponentLocation();
		FVector DotA = Location - EyePosition->GetComponentLocation();
		FVector DotB = EyePosition->GetForwardVector();

		DotA.Normalize();
		DotB.Normalize();

		AngleTo = UKismetMathLibrary::DegAcos(FVector::DotProduct(DotA, DotB));
	}

	AngleTo = 0.0f;
	Location = FVector::ZeroVector;
}

void ANPCStaticBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITORONLY_DATA
	SocketNames.Empty(1); SocketNames.Add(NAME_None);
	SocketNames = MeshComponent->GetAllSocketNames();
#endif
	
	LookAtComponent->SetRelativeLocation(LookAtLocation);
	if (!HeadSocketName.IsNone() && HeadSocketName != EyePosition->GetAttachSocketName() && MeshComponent->DoesSocketExist(HeadSocketName))
	{
		EyePosition->AttachToComponent(MeshComponent, FAttachmentTransformRules::KeepRelativeTransform, HeadSocketName);
	}
}
