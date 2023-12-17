// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DefaultSubobjects")
		USkeletalMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "DefaultSubobjects")
		UArrowComponent* EyePosition;
	
	UPROPERTY(BlueprintReadOnly, Category = "NPC")
		USceneComponent* LookAtComponent;
		
	UPROPERTY(VisibleDefaultsOnly, Category = "NPC", meta = (GetOptions = "SocketNames"))
		FName HeadSocketName;

	UPROPERTY(VisibleDefaultsOnly, Category = "NPC", meta = (MakeEditWidget = true))
		FVector LookAtLocation;

	UFUNCTION(BlueprintPure, Category = "NPC")
		void GetPlayerCameraInfo(float& AngleTo, FVector& Location) const;
	
	virtual FVector GetEyeWorldLocation_Implementation() override { return EyePosition->GetComponentLocation(); }
	virtual FVector GetEyeForwardVector_Implementation() override { return EyePosition->GetForwardVector(); }
	virtual USceneComponent* GetLookAtComponent_Implementation() override { return LookAtComponent; } 

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(meta = (TransientToolProperty)) TArray<FName> SocketNames = {};
#endif

	void OnConstruction(const FTransform& Transform) override;
};
