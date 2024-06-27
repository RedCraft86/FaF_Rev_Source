// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExecEnums.h"
#include "PlayerData.h"
#include "Data/MathTypes.h"
#include "Data/LightingData.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "FRPlayer.generated.h"

class AFREnemyBase;

#define FRPlayer(Context) AFRPlayerBase::Get<AFRPlayerBase>(Context)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyStackChangedSignature, const TArray<AFREnemyBase*>&, Enemies);

UCLASS(Abstract)
class FAF_REV_API AFRPlayerBase final : public ACharacter
{
	GENERATED_BODY()

	friend class UGameWidgetBase;

public:

	AFRPlayerBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<class USpringArmComponent> CameraArm;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<class UCameraComponent> PlayerCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<USceneComponent> EquipmentRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<UAudioComponent> FootstepAudio;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<UPointLightComponent> PlayerLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tick", meta = (ClampMin = 0.05f, UIMin = 0.05f))
		float SlowTickInterval;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerControlFlags"))
		int32 ControlFlags;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings", meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerStateFlags"))
		uint8 StateFlags;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
		TSet<FName> LockFlags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Interaction")
		TEnumAsByte<ECollisionChannel> InteractTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Interaction")
		float ReachDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		FVector2D Sensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
		FGTModifiableMultiplier SensitivityMulti;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		FGTModifiableSum FieldOfView;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		float FieldOfViewSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Camera|LockOn", meta = (DisplayThumbnail = false))
		TSoftObjectPtr<USceneComponent> LockOnTarget;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running|Stamina")
		FGTModifiableMultiplier StaminaDrainRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running|Stamina")
		FGTModifiableMultiplier StaminaGainRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running|Stamina")
		FPlayerStaminaDifficulty StaminaDifficulty;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		FPointLightProperties PlayerLightSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		TObjectPtr<AActor> UltraDynamicSky;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		TObjectPtr<class AInventoryPreview> InventoryPreview;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay, meta = (ReadOnlyKeys, DisplayThumbnail = false))
		TMap<FName, TObjectPtr<class UInputAction>> InputActions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		FPlayerCameraShakes CameraShakes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		FPlayerFootsteps FootstepSounds;

	UPROPERTY(BlueprintAssignable)
		FEnemyStackChangedSignature EnemyStackChanged;
	
protected:

	UPROPERTY(Transient) TObjectPtr<class AFRGameModeBase> GameMode;
	UPROPERTY(Transient) TObjectPtr<class AFRPlayerController> PlayerController;

	UPROPERTY(Transient) TObjectPtr<UObject> HidingSpot;
	UPROPERTY(Transient) TObjectPtr<UObject> WorldDevice;
	UPROPERTY(Transient) TObjectPtr<class ULevelSequencePlayer> ActiveCutscene;
	UPROPERTY(Transient) TSet<TObjectPtr<AFREnemyBase>> EnemyStack;

	float SlowTickTime;
	FVector CamPosition;
	FGTInterpScalar FOVValue;
	FGTInterpScalar HalfHeightValue;
	FPlayerInteraction InteractData;
	FRotator LockCurrentRot, LockTargetRot;
	FVector2D LeanCamOffset, SwayCamOffset;
	FVector2D CurrentCamOffset, TargetCamOffset;
	float MoveSpeedTarget, CurrentStamina, StaminaDelta;
	EPlayerLeanState LeanState;

	FTimerHandle StaminaTimer;
	FTimerHandle FootstepTimer;
	FTimerHandle WallDetectTimer;

public:

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Enemy Stack Changed")
		void EnemyStackChangedEvent(const EEnemyState PriorityMode);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void ResetStates();
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetPlayerSettings(const FPlayerSettings& InSettings);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void OverrideControlFlags(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerControlFlags")) const int32 InFlags);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetControlFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerControlFlags")) const int32 InFlag);
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void UnsetControlFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerControlFlags")) const int32 InFlag);
	
	UFUNCTION(BlueprintPure, Category = "Player")
		bool HasControlFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerControlFlags")) const int32 InFlag) const;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetStateFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerStateFlags")) const int32 InFlag);
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void UnsetStateFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerStateFlags")) const int32 InFlag);
	
	UFUNCTION(BlueprintPure, Category = "Player")
		bool HasStateFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerStateFlags")) const int32 InFlag) const;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddLockFlag(const FPlayerLockFlag InFlag);
	void AddLockFlag(const FName InFlag);
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void ClearLockFlag(const FPlayerLockFlag InFlag);
	void ClearLockFlag(const FName InFlag);

	UFUNCTION(BlueprintPure, Category = "Player")
		bool HasLockFlag(const FPlayerLockFlag InFlag) const;
	bool HasLockFlag(const FName InFlag) const;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetLightProperties(const FPointLightProperties& InProperties);

	UFUNCTION(BlueprintPure, Category = "Player")
		bool IsMoving() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetRunState(const bool bInState);

	UFUNCTION(BlueprintPure, Category = "Player")
		bool HasRunFlag() const;
	
	UFUNCTION(BlueprintPure, Category = "Player")
		bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetCrouchState(const bool bInState);

	UFUNCTION(BlueprintPure, Category = "Player")
		bool IsCrouching() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetLeanState(const EPlayerLeanState InState);

	UFUNCTION(BlueprintPure, Category = "Player")
		EPlayerLeanState GetLeanState() const;

	// Range: 0.0 -> 1.0
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetStaminaPercent(const float InStamina = 1.0f);

	// Range: 0.0 -> 1.0
	UFUNCTION(BlueprintPure, Category = "Player")
		float GetStaminaPercent() const;

	UFUNCTION(BlueprintPure, Category = "Player")
		bool IsStaminaPunished() const;

	UFUNCTION(BlueprintPure, Category = "Player")
		bool GetInteractionState(FPlayerInteraction& Data) const;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddSensitivityModifier(const FString InKey, const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void RemoveSensitivityModifier(const FString InKey);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddFieldOfViewModifier(const FString InKey, const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void RemoveFieldOfViewModifier(const FString InKey);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddMoveSpeedModifier(const FString InKey, const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void RemoveMoveSpeedModifier(const FString InKey);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddStaminaDrainModifier(const FString InKey, const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void RemoveStaminaDrainModifier(const FString InKey);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddStaminaGainModifier(const FString InKey, const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void RemoveStaminaGainModifier(const FString InKey);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetLockOnTarget(const USceneComponent* InComponent);

	UFUNCTION(BlueprintPure, Category = "Player")
		USceneComponent* GetLockOnTarget() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Player")
		void ForceExitHiding() const;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetHidingSpot(UObject* InObject);

	UFUNCTION(BlueprintPure, Category = "Player")
		UObject* GetHidingSpot() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Player")
		void ForceExitWorldDevice() const;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetWorldDevice(UObject* InObject);

	UFUNCTION(BlueprintPure, Category = "Player")
		UObject* GetWorldDevice() const;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddEnemy(AFREnemyBase* InEnemy);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void RemoveEnemy(AFREnemyBase* InEnemy);
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void ClearEnemyStack();
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Player")
		void FadeToBlack(const float InTime, const bool bAudio = true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Player")
		void FadeFromBlack(const float InTime, const bool bAudio = true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Player")
		void ClearFade() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void CutsceneStart(ULevelSequencePlayer* InSequence);
	
	UFUNCTION(BlueprintCallable, Category = "Player")
		void CutsceneEnd();
	
	UFUNCTION(BlueprintPure, Category = "Player")
		AFRGameModeBase* GetGameMode() const { return GameMode; }
	
	UFUNCTION(BlueprintPure, Category = "Player")
		AFRPlayerController* GetPlayerController() const { return PlayerController; }
	
	UFUNCTION(BlueprintPure, Category = "Player")
		ULevelSequencePlayer* GetActiveCutscene() const { return ActiveCutscene; }

	void TeleportPlayer(const FVector& InLocation, const FRotator& InRotation);
	virtual void SetActorHiddenInGame(bool bNewHidden) override;

protected:

	bool ShouldLock() const;
	bool IsGamePaused() const;
	bool IsStandingBlocked() const;
	bool IsLeaningBlocked(const float Direction) const;
	bool TraceInteraction(FHitResult& OutHitResult, FPlayerInteraction& OutData) const;
	
	void TickStamina();
	void TickFootstep();
	void LeanWallDetect();

	void InputBinding_Pause(const FInputActionValue& InValue);
	void InputBinding_Turn(const FInputActionValue& InValue);
	void InputBinding_Move(const FInputActionValue& InValue);
	void InputBinding_Run(const FInputActionValue& InValue);
	void InputBinding_Crouch(const FInputActionValue& InValue);
	void InputBinding_Lean(const FInputActionValue& InValue);
	void InputBinding_Inventory(const FInputActionValue& InValue);
	void InputBinding_HideQuests(const FInputActionValue& InValue);
	void InputBinding_Interact(const FInputActionValue& InValue);
	void InputBinding_Equipment(const FInputActionValue& InValue);
	void InputBinding_Equipment_Alt(const FInputActionValue& InValue);

	void OnSettingsApply();
	void OnDifficultyChanged(const EDifficultyMode InDifficulty);
	void SlowTick(const float DeltaTime);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
public: // Statics
	
	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Player (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject", AdvancedDisplay = "Class"))
		static AFRPlayerBase* GetPlayerSmart(const UObject* WorldContextObject, const TSubclassOf<AFRPlayerBase> Class);

	UFUNCTION(BlueprintCallable, Category = "Game", DisplayName = "Get Player (Checked)", meta = (DynamicOutputParam = "OutObject", DeterminesOutputType = "Class", ExpandEnumAsExecs = "ReturnValue", WorldContext = "WorldContextObject"))
		static EGTValidPins GetPlayerChecked(AFRPlayerBase*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AFRPlayerBase> Class);
	
	template <typename T = AFRPlayerBase>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetPlayerSmart(WorldContextObject, T::StaticClass()));
	}
};
