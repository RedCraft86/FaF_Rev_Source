﻿// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Interfaces/GCCharacterInterface.h"
#include "Interfaces/GCInteractionInterface.h"
#include "NPCStaticBase.generated.h"

UCLASS(DisplayName = "Static NPC Base")
class GAMECORE_API ANPCStaticBase final : public AActor, public IGCInteractionInterface, public IGCCharacterInterface
{
	GENERATED_BODY()

public:

	ANPCStaticBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		USkeletalMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "DefaultSubobjects")
		UArrowComponent* EyePosition;
	
	UPROPERTY(BlueprintReadOnly, Category = "NPC")
		USceneComponent* LookAtComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
		bool bCanInteract;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
		FText CharacterName;
	
	UPROPERTY(EditAnywhere, Category = "NPC", AdvancedDisplay, meta = (MakeEditWidget = true))
		FVector LookAtLocation;

	UPROPERTY(EditAnywhere, Category = "NPC", AdvancedDisplay, meta = (MakeEditWidget = true))
		FTransform EyeTransform;

	UFUNCTION(BlueprintPure, Category = "NPC")
		void GetPlayerCameraInfo(float& AngleTo, FVector& Location) const;

	virtual bool GetInteractionInfo_Implementation(FText& DisplayName) override;
	virtual FVector GetEyeWorldLocation_Implementation() override { return EyePosition->GetComponentLocation(); }
	virtual FVector GetEyeForwardVector_Implementation() override { return EyePosition->GetForwardVector(); }
	virtual USceneComponent* GetLookAtComponent_Implementation() override { return LookAtComponent; }

	virtual void GetSimpleCollisionCylinder(float& CollisionRadius, float& CollisionHalfHeight) const override;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient) bool bAlreadySpawned = false;
#endif
	
	virtual void OnConstruction(const FTransform& Transform) override;
};
