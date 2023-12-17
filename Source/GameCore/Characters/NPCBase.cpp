// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "NPCBase.h"
#include "Player/GCPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"

ANPCBase::ANPCBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	EyePosition = CreateDefaultSubobject<UArrowComponent>("EyePosition");
	EyePosition->SetupAttachment(GetMesh());

	LookAtComponent = CreateDefaultSubobject<USceneComponent>("LookAtComponent");
	LookAtComponent->SetupAttachment(GetMesh());
#if WITH_EDITORONLY_DATA
	LookAtComponent->bVisualizeComponent = true;
#endif

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

void ANPCBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITORONLY_DATA
	SocketNames.Empty(1); SocketNames.Add(NAME_None);
	SocketNames = GetMesh()->GetAllSocketNames();
#endif
	
	LookAtComponent->SetRelativeLocation(LookAtLocation);
	if (!HeadSocketName.IsNone() && HeadSocketName != EyePosition->GetAttachSocketName() && GetMesh()->DoesSocketExist(HeadSocketName))
	{
		EyePosition->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, HeadSocketName);
	}
}
