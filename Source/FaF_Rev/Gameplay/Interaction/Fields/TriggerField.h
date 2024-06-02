// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTVolume.h"
#include "WorldEvents/WorldEventComponent.h"
#include "TriggerField.generated.h"

class AFRPlayerBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFieldTriggerEvent, AFRPlayerBase*, Player);
DECLARE_MULTICAST_DELEGATE_OneParam(FFieldTriggerNativeEvent, AFRPlayerBase*);

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

	UPROPERTY(BlueprintAssignable, DisplayName = "On Triggered")
		FFieldTriggerEvent OnTriggeredEvent;
	FFieldTriggerNativeEvent OnTriggered;

private:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
