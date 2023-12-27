// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "CCTVCamera.h"
#include "Core/GCMiscTypes.h"
#include "InputActionValue.h"
#include "GameFramework/Actor.h"
#include "Interfaces/GCDeviceInterface.h"
#include "Interfaces/GCInteractionInterface.h"
#include "CCTVMonitor.generated.h"

class USphereComponent;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshEvent, bool, bEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCamChangeEvent, const int32, Index, const ACCTVCamera*, Camera);

UCLASS(Abstract, DisplayName = "CCTV Monitor")
class GAMECORE_API ACCTVMonitor final : public AActor, public IGCDeviceInterface, public IGCInteractionInterface
{
	GENERATED_BODY()

public:

	ACCTVMonitor();

	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, Category = "CCTV")
		bool bActive;

	UPROPERTY(EditAnywhere, Category = "CCTV", meta = (ClampMin = 200.0f, UIMin = 200.0f))
		FVector2D Range;

	UPROPERTY(EditAnywhere, Category = "CCTV", meta = (ClampMin = 128.0f, UIMin = 128.0f))
		FVector2D Resolution;
	
	UPROPERTY(EditAnywhere, Category = "CCTV")
		TArray<ACCTVCamera*> Cameras;

	UPROPERTY(EditAnywhere, Category = "CCTV", AdvancedDisplay)
		UMaterialInterface* ScreenMaterial;
	
	UPROPERTY(EditAnywhere, Category = "CCTV", AdvancedDisplay)
		UInputAction* ExitAction;

	UPROPERTY(EditAnywhere, Category = "CCTV", AdvancedDisplay)
		UInputAction* TurnAction;

	UPROPERTY(EditAnywhere, Category = "CCTV", AdvancedDisplay)
		float TurnSpeed;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Refresh")
		FOnRefreshEvent OnRefreshBP;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Camera Changed")
		FOnCamChangeEvent OnCamChangeBP;

	UFUNCTION(BlueprintImplementableEvent)
		UStaticMeshComponent* GetScreenMesh(uint8& MaterialIndex);

	UFUNCTION(BlueprintCallable, Category = "CCTV System")
		void RefreshCamera(const float WaitTime = 2.0f);
	
	UFUNCTION(BlueprintCallable, Category = "CCTV System")
		void ChangeCamera(const uint8 ToIndex = 0);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRefresh, bool);
	FOnRefresh OnRefresh;
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCamChange, int32, ACCTVCamera*);
	FOnCamChange OnCamChange;
	
private:
#if WITH_EDITOR
	USphereComponent* InnerSphere;
	USphereComponent* OuterSphere;
#endif

	UPROPERTY(Transient)
		AGCPlayerCharacter* Player;

	UPROPERTY(Transient)
		UMaterialInstanceDynamic* MID;

	UPROPERTY(Transient)
		class UCanvasRenderTarget2D* CVT;

	FTimerHandle SlowTick;
	FTimerHandle Refresh;
	FTimerHandle CamChange;
	float Static1InterpSpeed;
	float Static2InterpSpeed;
	FGCInterpolatableScalar Static1;
	FGCInterpolatableScalar Static2;
	TSet<TSoftObjectPtr<APawn>> Pawns;
    TSoftObjectPtr<ACCTVCamera> ActiveCamera;

	void DelayedTick();
	void UpdateRenderTarget();
	void InputBinding_Exit(const FInputActionValue& InValue);
	void InputBinding_Turn(const FInputActionValue& InValue);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void ForceExitDevice_Implementation(const FString& Reason) override;
	virtual bool GetInteractionInfo_Implementation(FText& DisplayName) override;
	virtual void OnBeginInteract_Implementation(AGCPlayerCharacter* InPlayer, const FHitResult& HitResult) override;
};
