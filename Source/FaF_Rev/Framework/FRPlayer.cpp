// Copyright (C) RedCraft86. All Rights Reserved.
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppParameterMayBeConst

#include "FRPlayer.h"
#include "FRGameMode.h"
#include "ExitInterface.h"
#include "UltraDynamicSky.h"
#include "FRPlayerController.h"
#include "Core/NarrativeWidget.h"
#include "Libraries/GTMathLibrary.h"
#include "SaveSystem/SaveSubsystem.h"
#include "GameSettings/GameSettings.h"
#include "Inventory/InventoryComponent.h"
#include "Interaction/InteractionInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "LevelSequencePlayer.h"
#include "EngineUtils.h"

#define INPUT_CHECK() !ShouldLock() && !IsGamePaused()
#define BIND_INPUT_ACTION(Component, Type, Event, Function) \
	if (const UInputAction* Action = InputActions.FindRef(Player::InputActions::Type)) \
	{ Component->BindAction(Action, ETriggerEvent::Event, this, &AFRPlayerBase::Function); }

AFRPlayerBase::AFRPlayerBase()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoReceiveInput = EAutoReceiveInput::Player0;
	
	CameraArm = CreateDefaultSubobject<USpringArmComponent>("CameraArm");
	CameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	CameraArm->SetupAttachment(GetCapsuleComponent());
	CameraArm->bEnableCameraRotationLag = true;
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->bDoCollisionTest = false;
	CameraArm->TargetArmLength = 0.0f;
	
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	PlayerCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	
	EquipmentRoot = CreateDefaultSubobject<USceneComponent>("EquipmentRoot");
	EquipmentRoot->SetupAttachment(PlayerCamera);
#if WITH_EDITOR
	EquipmentRoot->bVisualizeComponent = true;
#endif
	
	FootstepAudio = CreateDefaultSubobject<UAudioComponent>("FootstepAudio");
	FootstepAudio->SetRelativeLocation(FVector(0.0f, 0.0f, -60.0f));
	FootstepAudio->SetupAttachment(GetCapsuleComponent());

	PlayerLight = CreateDefaultSubobject<UPointLightComponent>("PlayerLight");
	PlayerLight->SetupAttachment(PlayerCamera);

	SlowTickInterval = 0.1f;
	
	ControlFlags = DEFAULT_PLAYER_CONTROL_FLAGS;
	StateFlags = 0;
	InteractTraceChannel = ECC_Visibility;
	ReachDistance = 250.0f;
	Sensitivity = FVector2D::UnitVector;
	SensitivityMulti = {1.0f};
	FieldOfView = {90.0f};
	FieldOfViewSpeed = 10.0f;
	LockOnTarget = nullptr;
	LockOnSpeed = 5.0f;
	MoveSpeedMultiplier = {1.0f};
	WalkingSpeed = 300.0f;
	CameraSway = {2.5f, 1.5f};
	RunningSpeed = 750.0f;
	RunningFOV = 5.0f;
	MaxStamina = 100.0f;
	StaminaDrainRate = {2.5f};
	StaminaGainRate = {1.75f};
	CrouchSpeed = 10.0f;
	CeilingTraceChannel = ECC_Visibility;
	HalfHeights = {88.0f, 45.0f};
	CrouchWalkSpeed = 200.0f;
	CrouchWalkFOV = -5.0f;
	LeanSpeed = 7.5f;
	WallTraceChannel = ECC_Visibility;
	WallTraceLength = 125.0f;
	LeanOffsets = {75.0f, 25.0f};
	PlayerLightSettings = {};
	InputActions = {};
	CameraShakes = {};
	FootstepSounds = {};

	GameMode = nullptr;
	PlayerController = nullptr;
	
	LockFlags = {};
	CamPosition = CameraArm->GetRelativeLocation();
	FOVValue = { FieldOfView.Evaluate() };
	HalfHeightValue = { FieldOfView.Evaluate() };
	InteractData = {};
	WorldDevice = nullptr;
	EnemyStack = {};
	LockCurrentRot = FRotator::ZeroRotator;
	LockTargetRot = FRotator::ZeroRotator;
	LeanCamOffset = FVector2D::ZeroVector;
	SwayCamOffset = FVector2D::ZeroVector;
	CurrentCamOffset = FVector2D::ZeroVector;
	TargetCamOffset = FVector2D::ZeroVector;
	MoveSpeedTarget = WalkingSpeed;
	CurrentStamina = MaxStamina;
	StaminaDelta = 0.0f;
	LeanState = EPlayerLeanState::None;


	PlayerLightSettings.Intensity = 0.025f;
	PlayerLightSettings.AttenuationRadius = 500.0f;
	PlayerLightSettings.bUseTemperature = true;
	PlayerLightSettings.Temperature = 12000.0f;
	PlayerLightSettings.bLightCastShadows = false;
	PlayerLightSettings.bUseInverseSquaredFalloff = true;
	PlayerLightSettings.LightFalloffExponent = 1.0f;
	PlayerLightSettings.SpecularScale = 0.0f;
	ULightingDataLibrary::SetPointLightProperties(PlayerLight, PlayerLightSettings);
}

void AFRPlayerBase::ResetStates()
{
	ClearEnemyStack();
	SetWorldDevice(nullptr);
	SetLockOnTarget(nullptr);
	
	SetRunState(false);
	SetCrouchState(false);
	SetLeanState(EPlayerLeanState::None);

	CutsceneEnd();
	ForceExitHiding();
	ForceExitWorldDevice();
	GameMode->Inventory->CloseUI();
	
	LockFlags.Remove(NAME_None);
	TSet<FName> RemovingLocks;
	for (const FName& Flag : LockFlags)
	{
		if (!Player::LockFlags::All.Contains(Flag)
			|| Player::LockFlags::CanReset.Contains(Flag))
			RemovingLocks.Add(Flag);
	}

	for (const FName& Flag : RemovingLocks)
	{
		LockFlags.Remove(Flag);
	}
	
	ControlFlags = DEFAULT_PLAYER_CONTROL_FLAGS;
	MoveSpeedMultiplier.Modifiers.Empty();
	FieldOfView.Modifiers.Empty();
	FOVValue.TargetValue = FieldOfView.Evaluate();
	FOVValue.SnapToTarget();
	HalfHeightValue.SnapToTarget();
	InteractData.Reset();
	SwayCamOffset = FVector2D::ZeroVector;
	StaminaDrainRate.Modifiers.Empty();
	StaminaGainRate.Modifiers.Empty();
	CurrentStamina = MaxStamina;
}

void AFRPlayerBase::SetPlayerSettings(const FPlayerSettings& InSettings)
{
	if (InSettings.bOverride_ControlFlags) OverrideControlFlags(InSettings.ControlFlags);
	if (InSettings.bOverride_LightProperties) SetLightProperties(InSettings.LightProperties);
	if (InSettings.bOverride_MoveSpeed) MoveSpeedMultiplier.BaseValue = InSettings.MoveSpeedMultiplier;
	if (InSettings.bOverride_StaminaRates)
	{
		StaminaDrainRate.BaseValue = InSettings.StaminaRates.X;
		StaminaGainRate.BaseValue = InSettings.StaminaRates.Y;
	}
}

void AFRPlayerBase::OverrideControlFlags(int32 InFlags)
{
	for (const EPlayerControlFlags Enum : TEnumRange<EPlayerControlFlags>())
	{
		if (InFlags & Enum)
		{
			SetControlFlag(Enum);
		}
		else
		{
			UnsetControlFlag(Enum);
		}
	}
}

void AFRPlayerBase::SetControlFlag(const int32 InFlag)
{
	if (InFlag == PCF_None) return;
	if (!HasControlFlag(InFlag))
	{
		ControlFlags |= InFlag;

		switch (InFlag)
		{
		case PCF_UseStamina:	GetWorldTimerManager().UnPauseTimer(StaminaTimer); break;
		default: return;
		}
	}
}

void AFRPlayerBase::UnsetControlFlag(const int32 InFlag)
{
	if (InFlag == PCF_None) return;
	if (HasControlFlag(InFlag))
	{
		ControlFlags &= ~InFlag;

		switch (InFlag)
		{
		case PCF_UseStamina:	GetWorldTimerManager().PauseTimer(StaminaTimer); break;
		case PCF_CanRun:		SetRunState(false); break;
		case PCF_CanCrouch:		SetCrouchState(false); break;
		case PCF_CanLean:		SetLeanState(EPlayerLeanState::None); break;
		default: return;
		}
	}
}

bool AFRPlayerBase::HasControlFlag(const int32 InFlag) const
{
	if (InFlag == PCF_None) return false;
	return ControlFlags & InFlag;
}

void AFRPlayerBase::SetStateFlag(const int32 InFlag)
{
	if (InFlag == PSF_None) return;
	StateFlags |= InFlag;
}

void AFRPlayerBase::UnsetStateFlag(const int32 InFlag)
{
	if (InFlag == PSF_None) return;
	StateFlags &= ~InFlag;
}

bool AFRPlayerBase::HasStateFlag(const int32 InFlag) const
{
	if (InFlag == PSF_None) return false;
	return StateFlags & InFlag;
}

void AFRPlayerBase::AddLockFlag(const FPlayerLockFlag InFlag)
{
	AddLockFlag(InFlag.SelectedValue);
}

void AFRPlayerBase::AddLockFlag(const FName InFlag)
{
	LockFlags.Remove(NAME_None);
 	if (!InFlag.IsNone()) LockFlags.Add(InFlag);
}

void AFRPlayerBase::ClearLockFlag(const FPlayerLockFlag InFlag)
{
	ClearLockFlag(InFlag.SelectedValue);
}

void AFRPlayerBase::ClearLockFlag(const FName InFlag)
{
	LockFlags.Remove(NAME_None);
 	if (!InFlag.IsNone()) LockFlags.Remove(InFlag);
}

bool AFRPlayerBase::HasLockFlag(const FPlayerLockFlag InFlag) const
{
	return HasLockFlag(InFlag.SelectedValue);
}

bool AFRPlayerBase::HasLockFlag(const FName InFlag) const
{
	return InFlag.IsNone() ? false : LockFlags.Contains(InFlag);
}

void AFRPlayerBase::SetLightProperties(const FPointLightProperties& InProperties)
{
	PlayerLightSettings = InProperties;
	ULightingDataLibrary::SetPointLightProperties(PlayerLight, PlayerLightSettings);
}

bool AFRPlayerBase::IsMoving() const
{
	// Test with 50 units just in case it's involuntary 
	return GetVelocity().Size2D() > 50.0f;
}

void AFRPlayerBase::SetRunState(const bool bInState)
{
	bool bRunning = HasRunFlag();
	if (bRunning != bInState)
	{
		const bool bCrouching = IsCrouching();
		bRunning = !IsStaminaPunished() && !bCrouching && bInState;
		if (bRunning)
		{
			SetStateFlag(PSF_Running);
		}
		else
		{
			UnsetStateFlag(PSF_Running);
		}

		if (!bCrouching)
		{
			MoveSpeedTarget = bRunning ? RunningSpeed : WalkingSpeed;
		}
	}
}

bool AFRPlayerBase::HasRunFlag() const
{
	return HasStateFlag(PSF_Running);
}

bool AFRPlayerBase::IsRunning() const
{
	return HasStateFlag(PSF_Running) && GetVelocity().Size() > WalkingSpeed;
}

void AFRPlayerBase::SetCrouchState(const bool bInState)
{
	bool bCrouching = IsCrouching();
	if (bCrouching != bInState)
	{
		const bool bRunning = IsRunning();
		bCrouching = !bRunning && bInState;
		if (bCrouching)
		{
			SetStateFlag(PSF_Crouching);
			AddFieldOfViewModifier(Player::InternalKeys::CrouchFOV, CrouchWalkFOV);
		}
		else
		{
			UnsetStateFlag(PSF_Crouching);
			RemoveFieldOfViewModifier(Player::InternalKeys::CrouchFOV);
		}

		HalfHeightValue.TargetValue = bCrouching ? HalfHeights.Y : HalfHeights.X;
		if (!bRunning)
		{
			MoveSpeedTarget = bCrouching ? CrouchWalkSpeed : WalkingSpeed;
		}
	}
}

bool AFRPlayerBase::IsCrouching() const
{
	return HasStateFlag(PSF_Crouching);
}

void AFRPlayerBase::SetLeanState(const EPlayerLeanState InState)
{
	if (LeanState == InState) return;
	if (InState == EPlayerLeanState::None)
	{
		LeanState = EPlayerLeanState::None;
		LeanCamOffset = FVector2D::ZeroVector;
		GetWorldTimerManager().PauseTimer(WallDetectTimer);
	}
	else
	{
		const float Dir = Player::LeanStateToFloat(InState);
		if (!IsLeaningBlocked(Dir))
		{
			LeanState = InState;
			LeanCamOffset = LeanOffsets * Dir;
			GetWorldTimerManager().UnPauseTimer(WallDetectTimer);
		}
	}
}

EPlayerLeanState AFRPlayerBase::GetLeanState() const
{
	return LeanState;
}

void AFRPlayerBase::SetStaminaPercent(const float InStamina)
{
	CurrentStamina = FMath::Lerp(0, MaxStamina, FMath::Clamp(InStamina, 0.0f, 1.0f));
}

float AFRPlayerBase::GetStaminaPercent() const
{
	return CurrentStamina / MaxStamina;
}

bool AFRPlayerBase::IsStaminaPunished() const
{
	return HasStateFlag(PSF_RunLocked);
}

bool AFRPlayerBase::GetInteractionState(FPlayerInteraction& Data) const
{
	Data = InteractData;
	return InteractData.IsValidData();
}

void AFRPlayerBase::AddSensitivityModifier(const FString InKey, const float InValue)
{
	SensitivityMulti.Modifiers.Add(InKey, InValue);
}

void AFRPlayerBase::RemoveSensitivityModifier(const FString InKey)
{
	SensitivityMulti.Modifiers.Remove(InKey);
}

void AFRPlayerBase::AddFieldOfViewModifier(const FString InKey, const float InValue)
{
	FieldOfView.Modifiers.Add(InKey, InValue);
}

void AFRPlayerBase::RemoveFieldOfViewModifier(const FString InKey)
{
	FieldOfView.Modifiers.Remove(InKey);
}

void AFRPlayerBase::AddMoveSpeedModifier(const FString InKey, const float InValue)
{
	MoveSpeedMultiplier.Modifiers.Add(InKey, InValue);
}

void AFRPlayerBase::RemoveMoveSpeedModifier(const FString InKey)
{
	MoveSpeedMultiplier.Modifiers.Remove(InKey);
}

void AFRPlayerBase::AddStaminaDrainModifier(const FString InKey, const float InValue)
{
	StaminaDrainRate.Modifiers.Add(InKey, InValue);
}

void AFRPlayerBase::RemoveStaminaDrainModifier(const FString InKey)
{
	StaminaDrainRate.Modifiers.Remove(InKey);
}

void AFRPlayerBase::AddStaminaGainModifier(const FString InKey, const float InValue)
{
	StaminaGainRate.Modifiers.Add(InKey, InValue);
}

void AFRPlayerBase::RemoveStaminaGainModifier(const FString InKey)
{
	StaminaGainRate.Modifiers.Remove(InKey);
}

void AFRPlayerBase::SetLockOnTarget(const USceneComponent* InComponent)
{
	if (InComponent) LockOnTarget = InComponent;
	else LockOnTarget.Reset();
}

USceneComponent* AFRPlayerBase::GetLockOnTarget() const
{
	return LockOnTarget.LoadSynchronous();
}

void AFRPlayerBase::ForceExitHiding() const
{
	IExitInterface::Exit(HidingSpot);
}

void AFRPlayerBase::SetHidingSpot(UObject* InObject)
{
	HidingSpot = InObject;
	if (HidingSpot) LockFlags.Add(Player::LockFlags::Hiding);
	else LockFlags.Remove(Player::LockFlags::Hiding);
}

UObject* AFRPlayerBase::GetHidingSpot() const
{
	return HidingSpot;
}

void AFRPlayerBase::ForceExitWorldDevice() const
{
	IExitInterface::Exit(WorldDevice);
}

void AFRPlayerBase::SetWorldDevice(UObject* InObject)
{
	WorldDevice = InObject;
	if (WorldDevice) LockFlags.Add(Player::LockFlags::WorldDevice);
	else LockFlags.Remove(Player::LockFlags::WorldDevice);
}

UObject* AFRPlayerBase::GetWorldDevice() const
{
	return WorldDevice;
}

void AFRPlayerBase::AddEnemy(AFREnemyBase* InEnemy)
{
	if (!EnemyStack.Contains(InEnemy))
	{
		EnemyStack.Add(InEnemy);
		EnemyStack.Remove(nullptr);
		EnemyStackChanged.Broadcast(EnemyStack.Array());
	}
}

void AFRPlayerBase::RemoveEnemy(AFREnemyBase* InEnemy)
{
	if (EnemyStack.Contains(InEnemy))
	{
		EnemyStack.Remove(InEnemy);
		EnemyStack.Remove(nullptr);
		EnemyStackChanged.Broadcast(EnemyStack.Array());
	}
}

void AFRPlayerBase::ClearEnemyStack()
{
	if (!EnemyStack.IsEmpty())
	{
		EnemyStack.Empty();
		EnemyStackChanged.Broadcast(EnemyStack.Array());
	}
}

void AFRPlayerBase::FadeToBlack(const float InTime, const bool bAudio) const
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		if (FMath::IsNearlyZero(InTime))
		{
			PlayerController->PlayerCameraManager->SetManualCameraFade(1.0f, FLinearColor::Black, bAudio);
		}
		else
		{
			PlayerController->PlayerCameraManager->StartCameraFade(0.0f, 1.0f,
			   InTime, FLinearColor::Black, bAudio, true);
		}
	}
}

void AFRPlayerBase::FadeFromBlack(const float InTime, const bool bAudio) const
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		if (FMath::IsNearlyZero(InTime))
		{
			PlayerController->PlayerCameraManager->SetManualCameraFade(0.0f, FLinearColor::Black, bAudio);
		}
		else
		{
			PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f,
			   InTime, FLinearColor::Black, bAudio, true);
		}
	}
}

void AFRPlayerBase::ClearFade() const
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->StopCameraFade();
	}
}

void AFRPlayerBase::CutsceneStart(ULevelSequencePlayer* InSequence)
{
	if (!InSequence) return;
	SetActorHiddenInGame(true);
	LockFlags.Add(Player::LockFlags::Cutscene);
	ActiveCutscene = InSequence;
}

void AFRPlayerBase::CutsceneEnd()
{
	if (!ActiveCutscene) return;
	SetActorHiddenInGame(false);
	LockFlags.Remove(Player::LockFlags::Cutscene);
	ActiveCutscene = nullptr;
}

void AFRPlayerBase::TeleportPlayer(const FVector& InLocation, const FRotator& InRotation)
{
	PlayerController->PlayerCameraManager->SetGameCameraCutThisFrame();
	SetActorLocation(InLocation, false, nullptr, ETeleportType::ResetPhysics);

	const bool bSmooth = CameraArm->bEnableCameraRotationLag;
	CameraArm->bEnableCameraRotationLag = false;
	FRotator Rot(InRotation); Rot.Roll = 0.0f;
	if (IsPlayerControlled())
	{
		if (AController* C = GetController())
		{
			C->SetControlRotation(Rot);
		}
	}
	else
	{
		Rot.Pitch = 0.0f;
		SetActorRotation(Rot, ETeleportType::ResetPhysics);
	}

	CameraArm->bEnableCameraRotationLag = bSmooth;
}

void AFRPlayerBase::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);
	EquipmentRoot->SetHiddenInGame(bNewHidden, true);
}

bool AFRPlayerBase::ShouldLock() const
{
	return ControlFlags & PCF_Locked || !LockFlags.IsEmpty();
}

bool AFRPlayerBase::IsGamePaused() const
{
	return GetWorldSettings()->GetPauserPlayerState() != nullptr;
}

bool AFRPlayerBase::IsStandingBlocked() const
{
	FVector Start, End = FVector::ZeroVector;
	UGTMathLibrary::GetActorLineTraceVectors(this, EVectorDirection::Up,
		GetCapsuleComponent()->GetUnscaledCapsuleRadius() + HalfHeights.X + 5.0f, Start, End);
        		
	FHitResult HitResult;
	return GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, CeilingTraceChannel,
		FCollisionShape::MakeSphere(10.0f), FCollisionQueryParams(NAME_None, false, this));
}

bool AFRPlayerBase::IsLeaningBlocked(const float Direction) const
{
	if (FMath::IsNearlyZero(Direction))
		return false;
	
	FVector Start, End = FVector::ZeroVector;
	UGTMathLibrary::GetActorLineTraceVectors(this, EVectorDirection::Right, WallTraceLength * Direction, Start, End);
	Start += FVector(0.0f, 0.0f, CameraArm->GetRelativeLocation().Z);
	End += FVector(0.0f, 0.0f, CameraArm->GetRelativeLocation().Z);

	FHitResult HitResult;
	return GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, WallTraceChannel,
		FCollisionShape::MakeSphere(10.0f), FCollisionQueryParams(NAME_None, false, this));
}

bool AFRPlayerBase::TraceInteraction(FHitResult& OutHitResult, FPlayerInteraction& OutData) const
{
	OutData.Reset();
	OutHitResult = {};

	if (ShouldLock()) return false;
	
	FVector Start, End = FVector::ZeroVector;
	UGTMathLibrary::GetComponentLineTraceVectors(PlayerCamera,
		EVectorDirection::Forward, ReachDistance, Start, End);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, InteractTraceChannel,
		FCollisionQueryParams(NAME_None, false, this)))
	{
		FInteractionInfo Info;
		AActor* Actor = HitResult.GetActor();
		if (IInteract::GetInteractionInfo(Actor, Info))
		{
			OutHitResult = HitResult;
			OutData.InteractInfo = Info;
			OutData.Target = Actor;
			return true;
		}
	}

	return false;
}

void AFRPlayerBase::TickStamina()
{
	StaminaDelta = IsRunning() ? -StaminaDrainRate.Evaluate() : StaminaGainRate.Evaluate();
	CurrentStamina = FMath::Clamp(StaminaDelta + CurrentStamina, 0.0f, MaxStamina);
	if (CurrentStamina < 0.01f && !IsStaminaPunished())
	{
		SetRunState(false);
		SetStateFlag(PSF_RunLocked);
	}
	else if (FMath::IsNearlyEqual(CurrentStamina, MaxStamina, 1.0f) && IsStaminaPunished())
	{
		UnsetStateFlag(PSF_RunLocked);
	}
}

void AFRPlayerBase::TickFootstep()
{
	FVector Start, End = FVector::ZeroVector;
	UGTMathLibrary::GetComponentLineTraceVectors(FootstepAudio, EVectorDirection::Up, -125.0f , Start, End);

	FCollisionQueryParams Params{NAME_None, true, this};
	Params.bReturnPhysicalMaterial = true;
		
	FHitResult HitResult;
#if WITH_EDITOR
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, FootstepSounds.FloorTraceChannel, Params))
	{
		// Box Brush check
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params) && !HitResult.GetActor())
		{
			HitResult.Reset();
		}
	}
#else
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, FootstepSounds.FloorTraceChannel, Params);
#endif
	if (IsMoving() && HitResult.bBlockingHit)
	{
		if (USoundBase* Sound = FootstepSounds.GetAudio(IsRunning(), IsCrouching(),
			UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get())))
		{
			FootstepAudio->SetVolumeMultiplier(FootstepSounds.Volume);
			FootstepAudio->SetSound(Sound);
			FootstepAudio->Play();
		}
	}
	
	FootstepTimer.Invalidate();
}

void AFRPlayerBase::LeanWallDetect()
{
	const float Dir = Player::LeanStateToFloat(LeanState);
	if (FMath::IsNearlyZero(Dir))
	{
		GetWorldTimerManager().PauseTimer(WallDetectTimer);
		return;
	}
	
	if (IsLeaningBlocked(Dir))
	{
		SetLeanState(EPlayerLeanState::None);
	}
}

void AFRPlayerBase::InputBinding_Pause(const FInputActionValue& InValue)
{
	if (LockFlags.Contains(Player::LockFlags::GuideScreen) || IsValid(ActiveCutscene)) return;
	if (LockFlags.Contains(Player::LockFlags::Inventory))
	{
		GameMode->Inventory->CloseUI();
		return;
	}

	if (IsGamePaused()) return;
	if (LockFlags.Contains(Player::LockFlags::WorldDevice))
	{
		ForceExitWorldDevice();
		return;
	}

	if (!ShouldLock())
	{
		SetRunState(false);
		SetLeanState(EPlayerLeanState::None);
		PlayerController->SetPauseState(true);
	}
}

void AFRPlayerBase::InputBinding_Turn(const FInputActionValue& InValue)
{
	if (INPUT_CHECK() && HasControlFlag(PCF_CanTurn) && (!LockOnTarget.IsValid() || LockOnTarget.IsNull()))
	{
		const FVector2D Axis = InValue.Get<FVector2D>();
		const float Multiplier = SensitivityMulti.Evaluate();
		if (!FMath::IsNearlyZero(Axis.X))
		{
			AddControllerYawInput(Axis.X * Sensitivity.X * Multiplier);
		}
		if (!FMath::IsNearlyZero(Axis.Y))
		{
			AddControllerPitchInput(Axis.Y * Sensitivity.Y * Multiplier * -1.0f);
		}
	}
}

void AFRPlayerBase::InputBinding_Move(const FInputActionValue& InValue)
{
	if (INPUT_CHECK() && HasControlFlag(PCF_CanMove))
	{
		const FVector2D Axis = InValue.Get<FVector2D>();
		if (!FMath::IsNearlyZero(Axis.X))
		{
			AddMovementInput(GetActorForwardVector(), Axis.X);
		}
		if (!FMath::IsNearlyZero(Axis.Y))
		{
			AddMovementInput(GetActorRightVector(), Axis.Y);
		}

		SwayCamOffset = CameraSway * Axis.Y;
	}
	else
	{
		SwayCamOffset = FVector2D::ZeroVector;
	}
}

void AFRPlayerBase::InputBinding_Run(const FInputActionValue& InValue)
{
	if (INPUT_CHECK())
	{
		SetRunState(HasControlFlag(PCF_CanRun) && InValue.Get<bool>());
	}
}

void AFRPlayerBase::InputBinding_Crouch(const FInputActionValue& InValue)
{
	if (INPUT_CHECK())
	{
		if (HasControlFlag(PCF_CanCrouch) && !IsCrouching())
		{
			SetCrouchState(true);
		}
		else if (!IsStandingBlocked())
		{
			SetCrouchState(false);
		}
	}
}

void AFRPlayerBase::InputBinding_Lean(const FInputActionValue& InValue)
{
	if (INPUT_CHECK() && (!LockOnTarget.IsValid() || LockOnTarget.IsNull()))
	{
		const float Direction = InValue.Get<float>();
		if (!HasControlFlag(PCF_CanLean) || FMath::IsNearlyZero(Direction))
		{
			SetLeanState(EPlayerLeanState::None);
		}
		else if (Direction > 0.0f)
		{
			SetLeanState(EPlayerLeanState::Right);
		}
		else // if (Direction < 0.0f)
		{
			SetLeanState(EPlayerLeanState::Left);
		}
	}
}

void AFRPlayerBase::InputBinding_Inventory(const FInputActionValue& InValue)
{
	if (LockFlags.Contains(Player::LockFlags::GuideScreen)) return;
	if (LockFlags.Contains(Player::LockFlags::Inventory))
	{
		GameMode->Inventory->CloseUI();
	}
	else if (INPUT_CHECK())
	{
		GameMode->Inventory->OpenUI();
	}
}

void AFRPlayerBase::InputBinding_HideQuests(const FInputActionValue& InValue)
{
	if (INPUT_CHECK())
	{
		if (UNarrativeWidgetBase* Widget = GameMode->GetWidget<UNarrativeWidgetBase>())
		{
			Widget->SetQuestsHidden(!Widget->AreQuestsHidden());
		}
	}
}

void AFRPlayerBase::InputBinding_Interact(const FInputActionValue& InValue)
{
	if (INPUT_CHECK() && HasControlFlag(PCF_CanInteract) && InValue.Get<bool>())
	{
		FHitResult HitResult;
		FPlayerInteraction NewInteract;
		if (TraceInteraction(HitResult, NewInteract))
		{
			if (InteractData != NewInteract)
			{
				IInteract::EndInteract(InteractData.Target, this);
				IInteract::BeginInteract(NewInteract.Target, this, HitResult);
				InteractData = NewInteract;
			}
			
			return;
		}
	}

	IInteract::EndInteract(InteractData.Target, this);
	InteractData.Reset();
}

void AFRPlayerBase::InputBinding_Equipment(const FInputActionValue& InValue)
{
	if (INPUT_CHECK())
	{
		GameMode->Inventory->EquipmentUse();
	}
}

void AFRPlayerBase::InputBinding_Equipment_Alt(const FInputActionValue& InValue)
{
	if (INPUT_CHECK())
	{
		GameMode->Inventory->EquipmentUseAlt(InValue.Get<bool>());
	}
}

void AFRPlayerBase::OnSettingsApply()
{
	const UGameSettings* Settings = UGameSettings::Get();
	CameraArm->bEnableCameraRotationLag = Settings->GetUseSmoothCamera();
	Sensitivity.X = Settings->GetSensitivityX();
	Sensitivity.Y = Settings->GetSensitivityY();
	FieldOfView.BaseValue = Settings->GetFieldOfView();

	FOVValue.TargetValue = FieldOfView.Evaluate();
	FOVValue.SnapToTarget();
	PlayerCamera->SetFieldOfView(FOVValue.CurrentValue);
}

void AFRPlayerBase::OnDifficultyChanged(const EDifficultyMode InDifficulty)
{
	AddStaminaDrainModifier(Player::InternalKeys::DifficultyStamina, StaminaDifficulty.GetDrain(InDifficulty));
	AddStaminaGainModifier(Player::InternalKeys::DifficultyStamina, StaminaDifficulty.GetGain(InDifficulty));
}

void AFRPlayerBase::SlowTick(const float DeltaTime)
{
	if (IsRunning() && IsMoving())
	{
		if (!FieldOfView.Modifiers.Contains(Player::InternalKeys::RunFOV))
		{
			AddFieldOfViewModifier(Player::InternalKeys::RunFOV, RunningFOV);
		}
	}
	else if (FieldOfView.Modifiers.Contains(Player::InternalKeys::RunFOV))
	{
		RemoveFieldOfViewModifier(Player::InternalKeys::RunFOV);
	}

	FOVValue.TargetValue = FieldOfView.Evaluate();
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeedTarget * MoveSpeedMultiplier.Evaluate();
}

void AFRPlayerBase::BeginPlay()
{
	Super::BeginPlay();
	LockFlags.Add(Player::LockFlags::Startup);
	
	FootstepSounds.LoadAssetsAsync();
	CamPosition = CameraArm->GetRelativeLocation();
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	if (const APlayerCameraManager* CM = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		EquipmentRoot->AttachToComponent(CM->GetTransformComponent(),
			FAttachmentTransformRules::KeepRelativeTransform);
	}

	PlayerController = FRPlayerController(this);

	GameMode = FRGameMode(this);
	GameMode->PlayerCharacter = this;
	GameMode->PlayerController = PlayerController;

	GameMode->Inventory->PlayerChar = this;
	GameMode->Inventory->SetInventoryPreview(InventoryPreview);

	FTimerManager& TM = GetWorldTimerManager();
	TM.SetTimer(StaminaTimer, this, &AFRPlayerBase::TickStamina, 0.1f, true);
	TM.SetTimer(WallDetectTimer, this, &AFRPlayerBase::LeanWallDetect, 0.1f, true);
	TM.PauseTimer(WallDetectTimer);
	if (!HasControlFlag(PCF_UseStamina))
	{
		TM.PauseTimer(StaminaTimer);
	}

	if (USaveSubsystem* SaveSystem = USaveSubsystem::Get(this))
	{
		OnDifficultyChanged(SaveSystem->GetDifficulty());
		SaveSystem->OnDifficultyChanged.AddUObject(this, &AFRPlayerBase::OnDifficultyChanged);
	}

	UGameSettings* Settings = UGameSettings::Get();
	Settings->OnManualApply.AddUObject(this, &AFRPlayerBase::OnSettingsApply);
	Settings->OnDynamicApply.AddUObject(this, &AFRPlayerBase::OnSettingsApply);

	LockFlags.Remove(Player::LockFlags::Startup);	
}

void AFRPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (SlowTickTime >= SlowTickInterval)
	{
		SlowTick(SlowTickTime);
		SlowTickTime = 0.0f;
	}
	else { SlowTickTime += DeltaTime; }

	if (!FOVValue.IsComplete())
	{
		PlayerCamera->SetFieldOfView(FOVValue.CurrentValue);
		FOVValue.InterpSpeed = FieldOfViewSpeed;
		FOVValue.Tick(DeltaTime);
	}
	
	if (!HalfHeightValue.IsComplete())
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(HalfHeightValue.CurrentValue);
		HalfHeightValue.InterpSpeed = CrouchSpeed;
		HalfHeightValue.Tick(DeltaTime);
	}

	if (const USceneComponent* LockOn = LockOnTarget.LoadSynchronous())
	{
		SetLeanState(EPlayerLeanState::None);
		LockCurrentRot = GetController()->GetControlRotation();
		LockTargetRot = FRotationMatrix::MakeFromX(LockOn->GetComponentLocation() - PlayerCamera->GetComponentLocation()).Rotator();
		GetController()->SetControlRotation(FMath::RInterpTo(LockCurrentRot, LockTargetRot, DeltaTime, LockOnSpeed));
	}
	else
	{
		TargetCamOffset = SwayCamOffset + LeanCamOffset;
		if (!CurrentCamOffset.Equals(TargetCamOffset, 0.01))
		{
			CurrentCamOffset = FMath::Vector2DInterpTo(CurrentCamOffset, TargetCamOffset, DeltaTime, LeanSpeed);
			CamPosition.Y = CurrentCamOffset.X;

			const FRotator CtrlRot = GetController()->GetControlRotation();
			GetController()->SetControlRotation(FRotator(CtrlRot.Pitch, CtrlRot.Yaw, CurrentCamOffset.Y));
		}

		if (IsMoving() && IsPlayerControlled())
		{
			const TSubclassOf<UCameraShakeBase> Shake = IsRunning() ? CameraShakes.RunShake : CameraShakes.WalkShake;
			const float Scale = IsRunning() ? CameraShakes.RunScale : CameraShakes.WalkScale;
			PlayerController->ClientStartCameraShake(Shake, Scale);
		}
	}

	const float HalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight_WithoutHemisphere();
	FootstepAudio->SetRelativeLocation({0.0f, 0.0f, -HalfHeight});
	
	const float CamHeight = HalfHeight + GetCapsuleComponent()->GetUnscaledCapsuleRadius() * 0.5f;
	CamPosition.Z = FMath::FInterpTo(CamPosition.Z, CamHeight, DeltaTime, 10.0f);

	CameraArm->SetRelativeLocation(CamPosition);
	
	if (!FootstepTimer.IsValid() && IsMoving())
	{
		GetWorldTimerManager().SetTimer(FootstepTimer, this, &AFRPlayerBase::TickFootstep,
			FootstepSounds.GetInterval(IsRunning(), IsCrouching()), false);
	}
}

void AFRPlayerBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	FootstepSounds.CheckEntries();
	StaminaDifficulty.CheckEntries();
	ULightingDataLibrary::SetPointLightProperties(PlayerLight, PlayerLightSettings);
	
	for (const FName& ActionName : Player::InputActions::All)
	{
		if (!InputActions.Contains(ActionName)) InputActions.Add(ActionName);
	}
	
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!IsValid(Actor)) continue;
		if (Actor->Implements<UUDSInterface>())
		{
			UltraDynamicSky = Actor;
		}

		if (Actor->IsA<AInventoryPreview>())
		{
			InventoryPreview = Cast<AInventoryPreview>(Actor);
		}
	}
}

void AFRPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		BIND_INPUT_ACTION(InputComp, Pause, Started, InputBinding_Pause);
		BIND_INPUT_ACTION(InputComp, Turn, Triggered, InputBinding_Turn);
		BIND_INPUT_ACTION(InputComp, Move, Triggered, InputBinding_Move);
		BIND_INPUT_ACTION(InputComp, Move, Completed, InputBinding_Move);
		BIND_INPUT_ACTION(InputComp, Move, Canceled, InputBinding_Move);
		BIND_INPUT_ACTION(InputComp, Run, Started, InputBinding_Run);
		BIND_INPUT_ACTION(InputComp, Run, Completed, InputBinding_Run);
		BIND_INPUT_ACTION(InputComp, Run, Canceled, InputBinding_Run);
		BIND_INPUT_ACTION(InputComp, Crouch, Started, InputBinding_Crouch);
		BIND_INPUT_ACTION(InputComp, Lean, Started, InputBinding_Lean);
		BIND_INPUT_ACTION(InputComp, Lean, Completed, InputBinding_Lean);
		BIND_INPUT_ACTION(InputComp, Lean, Canceled, InputBinding_Lean);
		BIND_INPUT_ACTION(InputComp, Inventory, Started, InputBinding_Inventory);
		BIND_INPUT_ACTION(InputComp, HideQuests, Started, InputBinding_HideQuests);
		BIND_INPUT_ACTION(InputComp, Interact, Triggered, InputBinding_Interact);
		BIND_INPUT_ACTION(InputComp, Interact, Completed, InputBinding_Interact);
		BIND_INPUT_ACTION(InputComp, Interact, Canceled, InputBinding_Interact);
		BIND_INPUT_ACTION(InputComp, Equipment, Started, InputBinding_Equipment);
		BIND_INPUT_ACTION(InputComp, Equipment_Alt, Started, InputBinding_Equipment_Alt);
		BIND_INPUT_ACTION(InputComp, Equipment_Alt, Completed, InputBinding_Equipment_Alt);
		BIND_INPUT_ACTION(InputComp, Equipment_Alt, Canceled, InputBinding_Equipment_Alt);
	}
}

/* Statics */

AFRPlayerBase* AFRPlayerBase::GetPlayerSmart(const UObject* WorldContextObject, const TSubclassOf<AFRPlayerBase> Class)
{
	AFRPlayerBase* Obj = nullptr;
#if WITH_EDITOR
	if (FApp::IsGame())
#endif
	{
		Obj = Cast<AFRPlayerBase>(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0));
	}

	if (!IsValid(Obj)) Obj = Cast<AFRPlayerBase>(UGameplayStatics::GetActorOfClass(WorldContextObject, StaticClass()));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}

EGTValidPins AFRPlayerBase::GetPlayerChecked(AFRPlayerBase*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AFRPlayerBase> Class)
{
	OutObject = GetPlayerSmart(WorldContextObject, Class);
	return IsValid(OutObject) ? EGTValidPins::Valid : EGTValidPins::NotValid;
}
