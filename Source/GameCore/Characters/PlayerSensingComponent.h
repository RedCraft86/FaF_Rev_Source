// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Components/ArrowComponent.h"
#include "PlayerSensingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerSensingEventSignature);

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class GAMECORE_API UPlayerSensingComponent : public UArrowComponent
{
	GENERATED_BODY()

public:

	UPlayerSensingComponent();

	UPROPERTY(EditAnywhere, Category = "SensingLogic")
		bool bEnabled;

	UPROPERTY(EditAnywhere, Category = "SensingLogic")
		bool bDetectOnlyMovement;

	UPROPERTY(EditAnywhere, Category = "SensingLogic", meta = (EditCondition = "bDetectOnlyMovement", ClampMin = 5.0f, UIMin = 5.0f))
		float MinMovementSpeed;
	
	UPROPERTY(EditAnywhere, Category = "SensingLogic", meta = (ClampMin = 20.0f, UIMin = 20.0f, ClampMax = 80.0f, UIMax = 80.0f))
		float InnerAngle;

	UPROPERTY(EditAnywhere, Category = "SensingLogic", meta = (ClampMin = 25.0f, UIMin = 25.0f, ClampMax = 90.0f, UIMax = 90.0f))
		float OuterAngle;
	
	UPROPERTY(EditAnywhere, Category = "SensingLogic", meta = (ClampMin = 20.0f, UIMin = 20.0f))
		float MaxDistance;

	UPROPERTY(EditAnywhere, Category = "SensingLogic", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float SenseTime;
	
	UPROPERTY(EditAnywhere, Category = "SensingLogic", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float LoseTime;

	UPROPERTY(BlueprintAssignable)
		FPlayerSensingEventSignature OnSeen;

	UPROPERTY(BlueprintAssignable)
		FPlayerSensingEventSignature OnLost;

	UFUNCTION(BlueprintCallable, Category = "PlayerSensing")
		void SetEnabled(const bool bInEnabled);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(Transient)
		APawn* PlayerChar;
	
	UPROPERTY(Transient)
		class UGCGameInstance* GameInst;

	bool bDetected;
	FTimerHandle AnnounceTimer;

	bool MoveCheck() const;
	bool TraceCheck() const;
	bool InOuterAngle() const;
	bool InInnerAngle() const;
	float AngleToPlayer() const;
	float DistanceToPlayer() const;
	void BroadcastEvent() const;
	
	virtual void BeginPlay() override;
};
