// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Interfaces/GCCharacterInterface.h"
#include "EnemyAIBase.generated.h"

UCLASS(DisplayName = "Enemy AI Base")
class GAMECORE_API AEnemyAIBase : public ACharacter, public IGCCharacterInterface
{
	GENERATED_BODY()

public:

	AEnemyAIBase();

	UPROPERTY(VisibleAnywhere, Category = "DefaultSubobjects")
		class UPlayerSensing* SensingComponent;

	UPROPERTY(VisibleAnywhere, Category = "DefaultSubobjects")
		class USMStateMachineComponent* LogicComponent;

	UFUNCTION(BlueprintNativeEvent)
		FName GetEyeAttachBone() const;
	FName GetEyeAttachBone_Implementation() const { return TEXT("head"); }
	
	virtual FVector GetEyeWorldLocation_Implementation() override;
	virtual FVector GetEyeForwardVector_Implementation() override;
	virtual USceneComponent* GetLookAtComponent_Implementation() override { return nullptr; }

private:

	virtual void OnConstruction(const FTransform& Transform) override;
};
