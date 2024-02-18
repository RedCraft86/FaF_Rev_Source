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
		class USMStateMachineComponent* LogicComponent;

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bStartEnabled;

	UFUNCTION(BlueprintNativeEvent)
		UPlayerSensingComponent* GetSensingComponent() const;
	virtual UPlayerSensingComponent* GetSensingComponent_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent)
		void GetRequirements(bool& Sensing, bool& StateMachine, bool& Ticking) const;
	void GetRequirements_Implementation(bool& Sensing, bool& StateMachine, bool& Ticking) const;

	UFUNCTION(BlueprintCallable, Category = "EnemyAI")
		void SetEnabled(const bool bInEnabled);

	UFUNCTION(BlueprintPure, Category = "EnemyAI")
		bool GetEnabled() const { return bEnabled; }
	
	virtual FVector GetEyeWorldLocation_Implementation() override;
	virtual FVector GetEyeForwardVector_Implementation() override;
	virtual USceneComponent* GetLookAtComponent_Implementation() override { return nullptr; }

private:

	bool bEnabled;

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
