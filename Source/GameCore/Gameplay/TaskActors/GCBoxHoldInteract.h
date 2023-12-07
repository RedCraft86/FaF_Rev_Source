// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "GCBoxInteract.h"
#include "Components/SphereComponent.h"
#include "GCBoxHoldInteract.generated.h"

UCLASS(DisplayName = "Box Hold Interact Task")
class GAMECORE_API AGCBoxHoldInteract : public AGCBoxInteract
{
	GENERATED_BODY()

public:

	AGCBoxHoldInteract();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float FillDuration;

	UFUNCTION(BlueprintImplementableEvent)
		void ShouldShowWidget(const bool bShouldShow);
	
	UFUNCTION(BlueprintPure, Category = "TaskActor")
		FORCEINLINE float GetPercent() const { return CompletePercent; }

	virtual void LockTask() override;
	virtual void ResetTask() override;
	virtual void OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult) override;
	virtual void OnEndInteract_Implementation(AGCPlayerCharacter* Player) override;
	virtual void Tick(float DeltaSeconds) override;
	
protected:

	void SetWidgetState(const bool bNewState);
	
	bool bShowingWidget;
	bool bBeingInteracted;
	float CompletePercent;
};

UCLASS(DisplayName = "Box Hold Interact Task (Ranged)")
class GAMECORE_API AGCBoxHoldInteractRanged : public AGCBoxHoldInteract
{
	GENERATED_BODY()

public:

	AGCBoxHoldInteractRanged();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UBoxComponent* RangeBox;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class USphereComponent* RangeSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Range", meta = (GetOptions = "Shapes"))
		FName ShapeType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Range", meta = (MakeEditWidget = true))
		FVector ShapeOrigin;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Range", meta = (MakeEditWidget = true))
		FVector MaxExtent;

	UFUNCTION(BlueprintPure, Category = "TaskActor")
		bool IsPlayerInRange() const;
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult) override;
	virtual void OnEndInteract_Implementation(AGCPlayerCharacter* Player) override;

private:

	UPROPERTY(meta = (TransientToolProperty))
		TArray<FName> Shapes = {TEXT("Box"), TEXT("Sphere")};
	
	float TickDelta;
	float ZCache;
};
