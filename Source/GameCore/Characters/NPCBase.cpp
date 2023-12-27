// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "NPCBase.h"
#include "Player/GCPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

ANPCBase::ANPCBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);

	GetMesh()->SetRelativeLocation(FVector{0.0f, 0.0f, -90.0f});
	GetMesh()->SetRelativeRotation(FRotator{0.0f, -90.0f, 0.0f});

	EyePosition = CreateDefaultSubobject<UArrowComponent>("EyePosition");
	EyePosition->SetRelativeLocation(FVector{0.0f, 0.0f, 90.0f});
	EyePosition->SetRelativeRotation(FRotator{0.0f, 90.0f, 0.0f});
	EyePosition->SetupAttachment(GetMesh());

	LookAtComponent = CreateDefaultSubobject<USceneComponent>("LookAtComponent");
	LookAtComponent->SetupAttachment(GetCapsuleComponent());
#if WITH_EDITORONLY_DATA
	LookAtComponent->bVisualizeComponent = true;
#endif

	bCanInteract = true;
	CharacterName = FText::GetEmpty();
	LookAtLocation = FVector{0.0f, 0.0f, 65.0f};
	EyeTransform = {FRotator::ZeroRotator, FVector{0.0f, 0.0f, 70.0f}, FVector::OneVector};
}

void ANPCBase::GetPlayerCameraInfo(float& AngleTo, FVector& Location) const
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

bool ANPCBase::GetInteractionInfo_Implementation(FText& DisplayName)
{
	DisplayName = CharacterName;
	return bCanInteract;
}

void ANPCBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	LookAtComponent->SetRelativeLocation(LookAtLocation);
	EyePosition->SetRelativeTransform(EyeTransform);
}
