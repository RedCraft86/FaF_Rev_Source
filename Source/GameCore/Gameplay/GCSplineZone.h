// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GCSplineZone.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnteredLeftZoneBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionFadeBP, const float, Transition);

UCLASS(NotBlueprintable, DisplayName = "Spline Zone")
class GAMECORE_API AGCSplineZone final : public AActor
{
	GENERATED_BODY()

public:

	AGCSplineZone();

	UPROPERTY(BlueprintAssignable, DisplayName = "On Entered Zone")
		FOnEnteredLeftZoneBP OnEnteredZoneBP;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Left Zone")
		FOnEnteredLeftZoneBP OnLeftZoneBP;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Transition")
		FOnTransitionFadeBP OnTransitionBP;

	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class USplineComponent* SplineComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class USphereComponent* BoundingSphere;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bFadeTransition;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (EditCondition = "bFadeTransition"))
		bool bDoubledRadius;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (EditCondition = "bFadeTransition", ClampMin = 5.0f, UIMin = 5.0f))
		float TransitionRadius;

	UFUNCTION(BlueprintPure, Category = "Spline Zone")
		FORCEINLINE bool IsPlayerInZone() const { return bPlayerInZone; }

	UFUNCTION(BlueprintPure, Category = "Spline Zone")
		FORCEINLINE float GetTransitionValue() const { return PlayerTransition; }
	
	DECLARE_MULTICAST_DELEGATE(OnEnteredLeftZone);
	OnEnteredLeftZone OnEnteredZone;
	OnEnteredLeftZone OnLeftZone;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTransitionFade, const float);
	FOnTransitionFade OnTransition;
	
private:

	UPROPERTY(Transient)
		AActor* PlayerChar;
	
	bool bPlayerInZone;
	float PlayerTransition;

	void UpdateZone(const bool bIsInZone);
	void UpdateFade(const float TransitionValue);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};
