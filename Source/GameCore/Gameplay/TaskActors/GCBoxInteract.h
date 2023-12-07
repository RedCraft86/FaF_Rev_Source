// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "GCTaskActorBase.h"
#include "RCPrimitiveTypes.h"
#include "Interfaces/GCInteractionInterface.h"
#include "GCBoxInteract.generated.h"

UCLASS(DisplayName = "Box Interact Task")
class GAMECORE_API AGCBoxInteract : public AGCTaskActorBase, public IGCInteractionInterface
{
	GENERATED_BODY()

public:

	AGCBoxInteract();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UBoxComponent* CollisionBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor")
		FText Label;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor", meta = (MakeEditWidget = true))
		FVector BoxExtent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor")
		FRCPrimitiveCollision Collision;
	
	virtual void LockTask() override;
	virtual void UnlockTask() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult) override;
	virtual bool GetInteractionInfo_Implementation(FText& DisplayName) override;

#if WITH_EDITOR
protected:
	UBillboardComponent* VisualIcon = nullptr;
#endif
};
