// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTVolume.h"
#include "Interaction/InteractionInterface.h"
#include "WorldEvents/WorldEventComponent.h"
#include "InteractField.generated.h"

UCLASS(meta = (AllowedCategories = "Collision, BrushSettings"))
class FAF_REV_API AInteractField final : public AGTVolume, public IInteractionInterface
{
	GENERATED_BODY()

public:

	AInteractField();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UWorldEventComponent> WorldEventComponent;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -1))
		bool bSingleUse;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings", meta = (DisplayPriority = -1))
		FInteractionInfo InteractionInfo;

private:
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult);
	virtual bool GetInteractionInfo_Implementation(FInteractionInfo& Info);
};
