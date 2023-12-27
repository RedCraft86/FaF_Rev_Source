// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GCTaskActorBase.h"
#include "GCBoxTrigger.generated.h"

UCLASS(DisplayName = "Box Trigger Task")
class GAMECORE_API AGCBoxTrigger : public AGCTaskActorBase
{
	GENERATED_BODY()

public:

	AGCBoxTrigger();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor", meta = (MakeEditWidget = true))
		FVector BoxExtent;

	virtual void LockTask() override;
	virtual void UnlockTask() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

#if WITH_EDITOR
protected:
	UBillboardComponent* VisualIcon = nullptr;
#endif
};
