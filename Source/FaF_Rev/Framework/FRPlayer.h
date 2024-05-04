// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "PlayerData.h"
#include "Data/MathTypes.h"
#include "Data/LightingData.h"
#include "GameFramework/Character.h"
#include "Inspection/InspectionActor.h"
#include "PhotoMode/PhotoModeActor.h"
#include "FRPlayer.generated.h"

#define FRPlayer(Context) AFRPlayerBase::Get<AFRPlayerBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRPlayerBase final : public ACharacter
{
	GENERATED_BODY()

public:

	AFRPlayerBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class USpringArmComponent* CameraArm;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* EquipmentRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UAudioComponent* FootstepAudio;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UPointLightComponent* PlayerLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerControlFlags"))
		int32 ControlFlags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Interaction")
		TEnumAsByte<ECollisionChannel> InteractTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Interaction")
		float ReachDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		FVector2D Sensitivity;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		FGTModifiableSum FieldOfView;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		float FieldOfViewSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Camera|LockOn")
		USceneComponent* LockOnTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera|LockOn")
		float LockOnSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
		FGTModifiableMultiplier MoveSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float WalkingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		FVector2D CameraSway;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Running", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float RunningSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running")
		float RunningFOV;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running|Stamina", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float MaxStamina;

	// X: Gain, Y: Drain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running|Stamina", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 5.0f, UIMax = 5.0f))
		FVector2D StaminaRates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float CrouchSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching")
		TEnumAsByte<ECollisionChannel> CeilingTraceChannel;

	// X: Standing, Y: Crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		FVector2D HalfHeights;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float CrouchWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching")
		float CrouchWalkFOV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float LeanSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning")
		TEnumAsByte<ECollisionChannel> WallTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float WallTraceLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning")
		FVector2D LeanOffsets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Misc")
		FPointLightProperties PlayerLightSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		TSoftObjectPtr<APhotoModeActor> PhotoModeActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		TSoftObjectPtr<AInspectionActor> InspectionActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay, meta = (ReadOnlyKeys, DisplayThumbnail = false))
		TMap<FName, class UInputAction*> InputActions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		FPlayerCameraShakes CameraShakes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		FPlayerFootsteps FootstepSounds;

protected:

	UPROPERTY(Transient) class AFRGameModeBase* GameMode;
	UPROPERTY(Transient) class AFRControllerBase* PlayerController;

	TSet<FName> LockFlags;
	FVector CamPosition;
	EPlayerLeanState LeanState;
	FGTInterpScalar FieldOfViewValue;
	FGTInterpScalar HalfHeightValue;
	FPlayerInteraction InteractData;
	TSoftObjectPtr<AActor> WorldDevice;
	TSet<TSoftObjectPtr<AActor>> EnemyStack;
	FVector2D LeanOffset, SwayOffset, CamOffset;
	float WalkSpeedTarget, CurrentStamina, StaminaDelta;
	bool bRunning, bCrouching, bStaminaPunished, bInteracting;
	
public:

	UFUNCTION(BlueprintCallable, Category = "Player")
		void ResetStates();
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetPlayerSettings(const FPlayerSettings& InSettings);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void OverrideControlFlags(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerControlFlags")) const int32 InFlags);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetControlFlag(const TEnumAsByte<EPlayerControlFlags> InFlag);
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void UnsetControlFlag(const TEnumAsByte<EPlayerControlFlags> InFlag);
	
	UFUNCTION(BlueprintPure, Category = "Player")
		bool HasControlFlag(const TEnumAsByte<EPlayerControlFlags> InFlag) const;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddLockFlag(const FPlayerLockState InFlag);
	void AddLockFlag(const FName InFlag);
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void ClearLockFlag(const FPlayerLockState InFlag);
	void ClearLockFlag(const FName InFlag);

	UFUNCTION(BlueprintPure, Category = "Player")
		bool HasLockFlag(const FPlayerLockState InFlag) const;
	bool HasLockFlag(const FName InFlag) const;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetLightProperties(const FPointLightProperties& InProperties);
	
	void TeleportPlayer(const FVector InLocation, const FRotator InRotation);
	virtual void SetActorHiddenInGame(bool bNewHidden) override;

protected:

	bool ShouldLock() const;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
public: // Statics
	
	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Player (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject"))
		static AFRPlayerBase* GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<AFRPlayerBase> Class);
	template <typename T = AFRPlayerBase>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetPlayerPawnSmart(WorldContextObject, T::StaticClass()));
	}
};
