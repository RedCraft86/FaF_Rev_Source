// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTActor.h"
#include "Components/BoxComponent.h"
#include "Interaction/InteractionInterface.h"
#include "WorldEvents/WorldEventComponent.h"
#include "InteractField.generated.h"

UCLASS()
class FAF_REV_API AInteractField final : public AGTActor, public IInteractionInterface
{
	GENERATED_BODY()

public:

	AInteractField();
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Subobjects")
		UWorldEventComponent* WorldEventComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (DisplayPriority = -1))
		bool bSingleUse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (DisplayPriority = -1))
		FText DisplayLabel;

private:

	virtual void OnEnableStateChange(const bool bIsEnabled) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void OnBeginInteract_Implementation(AFRPlayer* Player, const FHitResult& HitResult);
	virtual bool GetInteractionInfo_Implementation(FText& DisplayName);
};
