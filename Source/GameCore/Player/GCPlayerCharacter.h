// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Sound/SoundBase.h"
#include "Core/GCMiscTypes.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/WorldSettings.h"
#include "Interfaces/GCCharacterInterface.h"
#include "Interfaces/GCDeviceInterface.h"
#include "Interfaces/GCInteractionInterface.h"
#include "GCPlayerCharacter.generated.h"

class UInputAction;
class UCameraComponent;
class UGCUserSettings;
class UGCCameraSmoother;

UENUM(BlueprintType, DisplayName = "Player Input Types")
enum class EGCPlayerInputTypes : uint8
{
	Pause,
	Move,
	Turn,
	Run,
	Crouch,
	Lean,
	Inventory,
	HideQuests,
	Interact,
	CloseEyes,
	Equipment_Toggle,
	Equipment_Charge,

	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EGCPlayerInputTypes, EGCPlayerInputTypes::Max);

UENUM(BlueprintType, DisplayName = "Player Ability Flags", meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EGCPlayerAbilityFlags : uint8
{
	None		= 0 UMETA(Hidden),
	CloseEyes	= 1 << 0
};

ENUM_CLASS_FLAGS(EGCPlayerAbilityFlags);
ENUM_RANGE_BY_FIRST_AND_LAST(EGCPlayerAbilityFlags, EGCPlayerAbilityFlags::CloseEyes, EGCPlayerAbilityFlags::CloseEyes)
#define DEFAULT_PLAYER_ABILITIES /*(uint8)(EGCPlayerAbilityFlags::Foo)*/ 0

UENUM(BlueprintType, DisplayName = "Player Active State")
enum class EGCPlayerActiveState : uint8
{
	Normal,
	Device,
	Hiding,
	Inventory,
	Dialogue,
	Cutscene,
	Jumpscare,
	Loading,
};

UENUM(BlueprintType, DisplayName = "Player Lean State")
enum class EGCLeanState : uint8
{
	None,
	Left,
	Right
};

USTRUCT(BlueprintType, DisplayName = "Player Camera Shake Settings")
struct GAMECORE_API FGCPlayerCameraShakeSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCameraShakeSettings")
		TSubclassOf<UCameraShakeBase> WalkShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCameraShakeSettings", meta = (EditCondition = "WalkShake != nullptr"))
		float WalkShakeScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCameraShakeSettings")
		TSubclassOf<UCameraShakeBase> RunShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCameraShakeSettings", meta = (EditCondition = "RunShake != nullptr"))
		float RunShakeScale;

	FGCPlayerCameraShakeSettings()
		: WalkShake(nullptr)
		, WalkShakeScale(1.0f)
		, RunShake(nullptr)
		, RunShakeScale(0.75f)
	{}
};

USTRUCT(BlueprintType, DisplayName = "Player Footstep Sounds")
struct GAMECORE_API FGCPlayerFootstepSounds
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootstepSounds")
		TSoftObjectPtr<USoundBase> DefaultSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootstepSounds", meta = (ReadOnlyKeys))
		TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<USoundBase>> SurfaceSounds;

	FGCPlayerFootstepSounds() : DefaultSound(nullptr), SurfaceSounds({}) {}

	void CheckEntries()
	{
#if WITH_EDITOR
		if (SurfaceSounds.Contains(TEnumAsByte(SurfaceType_Default)))
		{
			TSoftObjectPtr<USoundBase> const* FoundPtr = SurfaceSounds.Find(TEnumAsByte(SurfaceType_Default));
			DefaultSound = DefaultSound.IsNull() && FoundPtr && !FoundPtr->IsNull() ? *FoundPtr : DefaultSound;
		}
		
		if (const UEnum* Enum = StaticEnum<EPhysicalSurface>())
		{
			for(int32 i = 1; i < Enum->NumEnums(); i++)
			{
				if(i != 0 && !Enum->HasMetaData(TEXT("Hidden"), i))
				{
					if (const EPhysicalSurface Type = static_cast<EPhysicalSurface>(i);
						!SurfaceSounds.Contains(TEnumAsByte(Type)))
					{
						SurfaceSounds.Add(TEnumAsByte(Type));
					}
				}
			}
		}
#endif
	}

	USoundBase* GetSound(const EPhysicalSurface Surface) const
	{
		TSoftObjectPtr<USoundBase> const* FoundPtr = SurfaceSounds.Find(TEnumAsByte(Surface));
		USoundBase* Sound = FoundPtr ? FoundPtr->LoadSynchronous() : nullptr;
		return Sound ? Sound : DefaultSound.LoadSynchronous();
	}
};

USTRUCT(BlueprintType, DisplayName = "Player Footstep Settings")
struct GAMECORE_API FGCPlayerFootstepSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootstepSettings")
		TEnumAsByte<ECollisionChannel> FloorTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootstepSettings", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		FVector Intervals;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootstepSettings", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Volume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootstepSettings")
		FGCPlayerFootstepSounds WalkSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootstepSettings")
		FGCPlayerFootstepSounds RunSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootstepSettings")
		FGCPlayerFootstepSounds SneakSounds;

	FGCPlayerFootstepSettings()
		: FloorTraceChannel(ECC_Visibility)
		, Intervals(FVector(0.5f, 0.35f, 0.6f))
		, Volume(1.0f)
		, WalkSounds({})
		, RunSounds({})
		, SneakSounds({})
	{}

	void CheckEntries()
	{
		WalkSounds.CheckEntries();
		RunSounds.CheckEntries();
		SneakSounds.CheckEntries();
	}

	float GetInterval(const bool bRun, const bool bSneak) const
	{
		return bRun ? Intervals.Y : bSneak ? Intervals.Z : Intervals.X;
	}

	USoundBase* GetAudio(const bool bRun, const bool bSneak, const EPhysicalSurface Surface) const
	{
		return bRun ? RunSounds.GetSound(Surface) : bSneak ? SneakSounds.GetSound(Surface) : WalkSounds.GetSound(Surface);
	}
};

UCLASS(Abstract, DisplayName = "Main Player Character")
class GAMECORE_API AGCPlayerCharacter : public ACharacter, public IGCCharacterInterface
{
	GENERATED_BODY()

	friend class UGCLoadingWidget;
	friend class UGCGameplayWidget;
	
public:

	AGCPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	static AGCPlayerCharacter* Get(const UObject* WorldContext);
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UGCCameraSmoother* CameraSmoother;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* EquipmentRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UAudioComponent* FootstepAudio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		EGCPlayerActiveState StartingState;
	
	UPROPERTY(/*EditAnywhere, */BlueprintReadOnly, Category = "Settings")
		EGCPlayerActiveState ActiveState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (Bitmask, BitmaskEnum = "/Script/GameCore.EGCPlayerAbilityFlags"))
		int32 AbilityFlags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bCanPause;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Interaction")
		bool bCanInteract;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Interaction")
		TEnumAsByte<ECollisionChannel> InteractTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Interaction")
		float ReachDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		bool bCanTurn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		FVector2D Sensitivity;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		FGCModifiableScalar FieldOfView;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		const USceneComponent* LockOnTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Camera")
		float LockOnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
		bool bCanMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float WalkingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		FVector2D CameraSway;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
		FGCModifiableScalar WalkMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running")
		bool bCanRun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Running", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float RunningSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running", DisplayName = "Running FOV Modifier")
		float RunningFOVModifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Stamina", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Stamina", DisplayName = "Consumption Percent", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 100.0f, UIMax = 100.0f))
		float StaminaDrain;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Stamina", DisplayName = "Regeneration Percent", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 100.0f, UIMax = 100.0f))
		FVector2D StaminaGain;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching")
		bool bCanCrouch;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching")
		TEnumAsByte<ECollisionChannel> CeilingTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float StandingHalfHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float CrouchingHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float CrouchWalkingSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float CrouchInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Crouching", DisplayName = "Crouching FOV Modifier")
		float CrouchingFOVModifier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning")
		bool bCanLean;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning")
		TEnumAsByte<ECollisionChannel> WallTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float WallTraceLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning")
		FVector2D LeanOffsets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Leaning", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float LeanInterpSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
		class AGCPhotoModeActor* PhotoModeActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
		class AGCInspectionActor* InspectionActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
		TSubclassOf<class UGCEyeShutWidget> EyeShutWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
		FGCPlayerCameraShakeSettings CameraShakes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
		FGCPlayerFootstepSettings FootstepSounds;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References", meta = (ReadOnlyKeys))
		TMap<EGCPlayerInputTypes, UInputAction*> InputActions;

	DECLARE_MULTICAST_DELEGATE(FOnEyeClosedStateChange);
	FOnEyeClosedStateChange OnEyesClosed;
	FOnEyeClosedStateChange OnEyesOpened;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChange, const EGCPlayerActiveState);
	FOnPlayerStateChange OnStateChanged;

	bool IsInInvincibleState() const;
	
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual bool TeleportTo(const FVector& DestLocation, const FRotator& DestRotation, bool bIsATest = false, bool bNoCheck = false) override;

	virtual FVector GetEyeWorldLocation_Implementation() override { return PlayerCamera->GetComponentLocation(); }
	virtual FVector GetEyeForwardVector_Implementation() override { return PlayerCamera->GetForwardVector(); }
	virtual USceneComponent* GetLookAtComponent_Implementation() override { return PlayerCamera; }

	virtual void InputBinding_Pause(const FInputActionValue& InValue);
	virtual void InputBinding_Move(const FInputActionValue& InValue);
	virtual void InputBinding_Turn(const FInputActionValue& InValue);
	virtual void InputBinding_Run(const FInputActionValue& InValue);
	virtual void InputBinding_Crouch(const FInputActionValue& InValue);
	virtual void InputBinding_Lean(const FInputActionValue& InValue);
	virtual void InputBinding_Inventory(const FInputActionValue& InValue);
	virtual void InputBinding_HideQuests(const FInputActionValue& InValue);
	virtual void InputBinding_Interact(const FInputActionValue& InValue);
	virtual void InputBinding_CloseEyes(const FInputActionValue& InValue);
	virtual void InputBinding_Equipment_Toggle(const FInputActionValue& InValue);
	virtual void InputBinding_Equipment_Charge(const FInputActionValue& InValue);

	virtual void SetRunState(const bool bNewState);
	virtual void SetCrouchState(const bool bNewState);
	virtual void SetLeanState(const EGCLeanState NewState);
	virtual void SetEyesCloseState(const bool bNewState);

	class AGCPlayerController* GetPC() const { return PlayerController; }

protected:
#if WITH_EDITOR
	UBillboardComponent* VisualIcon;
#endif

	UPROPERTY(Transient)
		UGCEyeShutWidget* EyeShutWidget;

	UPROPERTY(Transient)
		AActor* ActiveWorldDevice;
	
	UPROPERTY(Transient)
		FGCInteractionData InteractionData;

	UPROPERTY(Transient)
		TArray<AActor*> EnemyStack;

	UPROPERTY(Transient)
		UGCLoadingWidget* LoadingWidget;

	UPROPERTY(Transient)
		class AGCPlayerController* PlayerController;
	
	bool bRunning;
	bool bCrouching;
	EGCLeanState LeanState;
	float MaxWalkSpeedTarget;
	FGCInterpolatableScalar FieldOfViewValue;
	FGCInterpolatableScalar HalfHeightValue;
	FVector2D CurrentCameraOffset;
	FVector2D TargetCameraSway;
	FVector2D TargetLeanOffset;
	FVector CamRelativePos;
	float InternalFOVMod;
	float CurrentStamina;
	bool bStaminaPunished;
	bool bHaveEyesClosed;
	bool bShouldBeInteracting;

	FTimerHandle WindowFocusTimer;
	FTimerHandle StaminaChangeTimer;
	FTimerHandle WallDetectionTimer;
	FTimerHandle FootstepTimer;
	
	virtual void BeginPlay() override;
	void OnDynamicSettingsApply(UGCUserSettings* InUserSettings);
	void OnManualSettingsApply(UGCUserSettings* InUserSettings);
	void OnWindowFocusChanged(const bool bFocused);

	bool IsGamePaused() const { return GetWorldSettings()->GetPauserPlayerState() != NULL; }
	bool TraceInteraction(FHitResult& OutHitResult, FGCInteractionData& OutData) const;
	bool IsLeaningBlocked(const float Direction) const;
	bool IsStandingBlocked() const;

	void TickWindowFocus();
	void TickStamina();
	void LeanWallDetection();
	void TickFootsteps();

public:
	
	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		FORCEINLINE bool IsMoving() const { return GetVelocity().Size() >= 5.0f; }

	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		FORCEINLINE bool IsRunning() const { return bRunning; }

	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		FORCEINLINE bool IsCrouching() const { return bCrouching; }

	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		FORCEINLINE EGCLeanState GetLeanState() const { return LeanState; }

	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		FORCEINLINE bool AreEyesClosed() const { return bHaveEyesClosed; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		FORCEINLINE FGCInteractionData GetInteractionData() const { return InteractionData; }

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		bool IsOnTaskOrDeviceActor() const;
	
	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		bool InMovementAction() const;
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		FORCEINLINE void SetCanPause(const bool bInCanPause) { bCanPause = bInCanPause; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetStaminaPercent(const float InPercent = 1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		FORCEINLINE float GetStaminaPercent() const { return CurrentStamina / MaxStamina; }

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		FORCEINLINE bool IsStaminaPunished() const { return bStaminaPunished; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetActiveState(const EGCPlayerActiveState InState);

	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		FORCEINLINE EGCPlayerActiveState GetActiveState() const { return ActiveState; }

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetAbilityFlags(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/GameCore.EGCPlayerAbilityFlags")) const int32 InFlags);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void EnableAbility(const EGCPlayerAbilityFlags InAbility);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void DisableAbility(const EGCPlayerAbilityFlags InAbility);

	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		FORCEINLINE bool HasAbility(const EGCPlayerAbilityFlags Ability) const { return AbilityFlags & static_cast<uint8>(Ability); }

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetCanInteract(const bool bInValue);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetCanTurn(const bool bInValue);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetSensitivity(const FVector2D InValue = FVector2D::UnitVector);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetCanMove(const bool bInValue);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetCanRun(const bool bInValue);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetCanCrouch(const bool bInValue);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetCanLean(const bool bInValue);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetFieldOfView(const float InValue = 90.0f);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void AddFieldOfViewModifier(const FName InKey, const float InValue);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void RemoveFieldOfViewModifier(const FName InKey);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetWalkMultiplier(const float InValue = 1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void AddWalkMultiplierModifier(const FName InKey, const float InValue = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void RemoveWalkMultiplierModifier(const FName InKey);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetLockOnTarget(const USceneComponent* InComponent) { LockOnTarget = InComponent; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void ResetPlayer();

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void SetWorldDevice(AActor* InActor);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		AActor* GetWorldDevice() const { return ActiveWorldDevice; }
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void AddEnemy(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void RemoveEnemy(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
		void ClearEnemies() { EnemyStack.Empty(); }
	
	UFUNCTION(BlueprintPure, Category = "PlayerCharacter")
		const TArray<AActor*>& GetEnemyStack();
};
