﻿// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "NPCStaticBase.h"
#include "Core/CoreLogging.h"
#include "Player/GCPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

ANPCStaticBase::ANPCStaticBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->bDynamicObstacle = true;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionObjectType(ECC_Pawn);
	SetRootComponent(CapsuleComponent);
	
	MeshComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	MeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	MeshComponent->bAffectDynamicIndirectLighting = true;
	MeshComponent->bCastDynamicShadow = true;
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetCollisionProfileName(TEXT("CharacterMesh"));
	MeshComponent->SetRelativeLocation(FVector{0.0f, 0.0f, -90.0f});
	MeshComponent->SetRelativeRotation(FRotator{0.0f, -90.0f, 0.0f});
	MeshComponent->SetupAttachment(CapsuleComponent);
	
	EyePosition = CreateDefaultSubobject<UArrowComponent>("EyePosition");
	EyePosition->SetupAttachment(CapsuleComponent);

	LookAtComponent = CreateDefaultSubobject<USceneComponent>("LookAtComponent");
	LookAtComponent->SetupAttachment(CapsuleComponent);
#if WITH_EDITORONLY_DATA
	LookAtComponent->bVisualizeComponent = true;
#endif

	bCanInteract = true;
	CharacterName = FText::GetEmpty();
	LookAtLocation = FVector{0.0f, 0.0f, 65.0f};
	EyeTransform = {FRotator::ZeroRotator, FVector{0.0f, 0.0f, 70.0f}, FVector::OneVector};
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
		return;
	}

	AngleTo = 0.0f;
	Location = FVector::ZeroVector;
}

bool ANPCStaticBase::GetInteractionInfo_Implementation(FText& DisplayName)
{
	DisplayName = CharacterName;
	return bCanInteract;
}

void ANPCStaticBase::GetSimpleCollisionCylinder(float& CollisionRadius, float& CollisionHalfHeight) const
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (IsTemplate())
	{
		UE_LOG(GameCore, Log, TEXT("WARNING ANPCStaticBase::GetSimpleCollisionCylinder : Called on default object '%s'. Will likely return zero size. Consider using GetDefaultHalfHeight() instead."), *this->GetPathName());
	}
#endif

	if (RootComponent == CapsuleComponent && IsRootComponentCollisionRegistered())
	{
		// Note: we purposefully ignore the component transform here aside from scale, always treating it as vertically aligned.
		// This improves performance and is also how we stated the CapsuleComponent would be used.
		CapsuleComponent->GetScaledCapsuleSize(CollisionRadius, CollisionHalfHeight);
	}
	else
	{
		Super::GetSimpleCollisionCylinder(CollisionRadius, CollisionHalfHeight);
	}
}

void ANPCStaticBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITORONLY_DATA
	// Spawn behavior that imitates ACharacter
	if (!FApp::IsGame() && !bAlreadySpawned)
	{
		FHitResult HitResult;
		const TSet Channels = {ECC_WorldStatic, ECC_WorldDynamic, ECC_PhysicsBody, ECC_Destructible};
		for (const ECollisionChannel& Channel : Channels)
		{
			if (GetWorld()->LineTraceSingleByChannel(HitResult,
				Transform.GetTranslation() + FVector{0.0f, 0.0f, 10.0f},
				Transform.GetTranslation() - FVector{0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight()},
				Channel, {TEXT("StaticNPC_Floor_Test"), true, this}))
			{
				AddActorWorldOffset(FVector{0.0f, 0.0f, FVector::Dist(HitResult.Location, HitResult.TraceEnd)});
				break;
			}
		}
		
		bAlreadySpawned = true;
	}
#endif
	
	LookAtComponent->SetRelativeLocation(LookAtLocation);
	EyePosition->SetRelativeTransform(EyeTransform);
}
