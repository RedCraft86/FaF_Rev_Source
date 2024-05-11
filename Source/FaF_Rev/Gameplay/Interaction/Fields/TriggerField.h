// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTVolume.h"
#include "WorldEvents/WorldEventComponent.h"
#include "TriggerField.generated.h"

UCLASS(meta = (AllowedCategories = "Collision, BrushSettings"))
class FAF_REV_API ATriggerField final : public AGTVolume
{
	GENERATED_BODY()

public:

	ATriggerField();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UWorldEventComponent> WorldEventComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (DisplayPriority = -1))
		bool bSingleUse;

private:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
