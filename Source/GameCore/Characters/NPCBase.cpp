// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

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

	CharacterName = FText::GetEmpty();
	bCanInteract = true;
	HeadSocketName = NAME_None;
	LookAtLocation = FVector::ZeroVector;
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
#if WITH_EDITORONLY_DATA
	if (!FApp::IsGame())
	{
		SocketNames.Empty(1); SocketNames.Add(NAME_None);
		SocketNames = GetMesh()->GetAllSocketNames();
	}
#endif
	
	LookAtComponent->SetRelativeLocation(LookAtLocation);
	if (HeadSocketName != EyePosition->GetAttachSocketName())
	{
		EyePosition->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, HeadSocketName);
	}

#if WITH_EDITORONLY_DATA
	if (!FApp::IsGame() && bTryApplyAttachedTransform)
	{
		EyePosition->SetRelativeTransform(
			FTransform{
				FRotator{90.0f, 0.0f, 0.0f},
				FVector{0.0f, -10.0f, 0.0f},
				FVector{0.5f, 0.5f, 0.5f}
			});
		
		bTryApplyAttachedTransform = false;
	}
#endif
}
