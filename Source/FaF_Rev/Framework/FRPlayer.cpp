// Copyright (C) RedCraft86. All Rights Reserved.
// ReSharper disable CppParameterMayBeConst

#include "FRPlayer.h"
#include "FRGameMode.h"
#include "FRPlayerController.h"
#include "ForceExitInterface.h"
#include "Libraries/GTMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#define RUN_FOV_KEY		FName(TEXT("Internal_RunFOV"))
#define CROUCH_FOV_KEY	FName(TEXT("Internal_CrouchFOV"))
#define CHASE_FOV_KEY	FName(TEXT("Internal_ChaseFOV"))

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
	PlayerLight->SetupAttachment(GetCapsuleComponent());

	SlowTickInterval = 0.25f;
	
	ControlFlags = DEFAULT_PLAYER_CONTROL_FLAGS;
	StateFlags = 0;
	InteractTraceChannel = ECC_Visibility;
	ReachDistance = 250.0f;
	Sensitivity = FVector2D::UnitVector;
	FieldOfView = {90.0f};
	FieldOfViewSpeed = 10.0f;
	ChaseFOV = 2.0f;
	LockOnTarget = nullptr;
	LockOnSpeed = 5.0f;
	MoveSpeedMultiplier = {1.0f};
	WalkingSpeed = 300.0f;
	CameraSway = {2.5f, 1.5f};
	RunningSpeed = 750.0f;
	RunningFOV = 5.0f;
	MaxStamina = 100.0f;
	StaminaRates = {2.5f, 1.75f};
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
	PhotoModeActor = nullptr;
	InspectionActor = nullptr;
	InputActions = {};
	CameraShakes = {};
	FootstepSounds = {};

	GameMode = nullptr;
	PlayerController = nullptr;
	
	LockFlags = {};
	CamPosition = FVector::ZeroVector;
	FOVValue = { FieldOfView.Evaluate() };
	HalfHeightValue = { FieldOfView.Evaluate() };
	InteractData = {};
	WorldDevice = nullptr;
	EnemyStack = {};
	LeanCamOffset = FVector2D::ZeroVector;
	SwayCamOffset = FVector2D::ZeroVector;
	CamOffset = FVector2D::ZeroVector;
	MoveSpeedTarget = WalkingSpeed;
	CurrentStamina = MaxStamina;
	StaminaDelta = 0.0f;
	LeanState = EPlayerLeanState::None;
}

void AFRPlayerBase::ResetStates()
{
	ClearEnemyStack();
	SetWorldDevice(nullptr);
	SetLockOnTarget(nullptr);
	
	SetRunState(false);
	SetCrouchState(false);
	SetLeanState(EPlayerLeanState::None);
	
	LockFlags.Remove(NAME_None);
	for (const FName& Flag : Player::LockFlags::CanReset)
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
	CurrentStamina = MaxStamina;
}

void AFRPlayerBase::SetPlayerSettings(const FPlayerSettings& InSettings)
{
	OverrideControlFlags(InSettings.ControlFlags);
	SetLightProperties(InSettings.LightProperties);
	MoveSpeedMultiplier.BaseValue = InSettings.MoveSpeedMultiplier;
	StaminaRates = InSettings.StaminaRates;
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

void AFRPlayerBase::SetControlFlag(const TEnumAsByte<EPlayerControlFlags> InFlag)
{
	if (!HasControlFlag(InFlag))
	{
		ControlFlags |= InFlag.GetValue();

		switch (InFlag)
		{
		case PCF_UseStamina:	GetWorldTimerManager().UnPauseTimer(StaminaTimer); break;
		default: return;
		}
	}
}

void AFRPlayerBase::UnsetControlFlag(const TEnumAsByte<EPlayerControlFlags> InFlag)
{
	if (HasControlFlag(InFlag))
	{
		ControlFlags &= ~InFlag.GetValue();

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

bool AFRPlayerBase::HasControlFlag(const TEnumAsByte<EPlayerControlFlags> InFlag) const
{
	return ControlFlags & InFlag.GetValue();
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
	bool bRunning = IsRunning();
	if (bRunning != bInState)
	{
		const bool bCrouching = IsCrouching();
		bRunning = !IsStaminaPunished() && !bCrouching && bInState;
		if (bRunning)
		{
			StateFlags |= PLAYER_STATE_FLAG(PCF_Running);
		}
		else
		{
			StateFlags &= ~PLAYER_STATE_FLAG(PCF_Running);
		}

		if (!bCrouching)
		{
			MoveSpeedTarget = bRunning ? RunningSpeed : WalkingSpeed;
		}
	}
}

bool AFRPlayerBase::IsRunning() const
{
	return StateFlags & PLAYER_STATE_FLAG(PCF_Running);
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
			StateFlags |= PLAYER_STATE_FLAG(PCF_Crouching);
			AddFieldOfViewModifier(CROUCH_FOV_KEY, CrouchWalkFOV);
		}
		else
		{
			StateFlags &= ~PLAYER_STATE_FLAG(PCF_Crouching);
			RemoveFieldOfViewModifier(CROUCH_FOV_KEY);
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
	return StateFlags & PLAYER_STATE_FLAG(PCF_Crouching);
}

void AFRPlayerBase::SetLeanState(const EPlayerLeanState InState)
{
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
	return StateFlags & PLAYER_STATE_FLAG(PCF_RunLocked);
}

bool AFRPlayerBase::GetInteractionState(FPlayerInteraction& Data) const
{
	Data = InteractData;
	return StateFlags & PLAYER_STATE_FLAG(PCF_Interacting);
}

void AFRPlayerBase::AddFieldOfViewModifier(const FName InKey, const float InValue)
{
	FieldOfView.Modifiers.Add(InKey, InValue);
}

void AFRPlayerBase::RemoveFieldOfViewModifier(const FName InKey)
{
	FieldOfView.Modifiers.Remove(InKey);
}

void AFRPlayerBase::AddMoveSpeedModifier(const FName InKey, const float InValue)
{
	MoveSpeedMultiplier.Modifiers.Add(InKey, InValue);
}

void AFRPlayerBase::RemoveMoveSpeedModifier(const FName InKey)
{
	MoveSpeedMultiplier.Modifiers.Remove(InKey);
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
	IForceExitInterface::Exit(HidingSpot.LoadSynchronous());
}

void AFRPlayerBase::SetHidingSpot(const UObject* InObject)
{
	HidingSpot = InObject;
}

UObject* AFRPlayerBase::GetHidingSpot() const
{
	return HidingSpot.LoadSynchronous();
}

void AFRPlayerBase::ForceExitWorldDevice() const
{
	IForceExitInterface::Exit(WorldDevice.LoadSynchronous());
}

void AFRPlayerBase::SetWorldDevice(const UObject* InObject)
{
	if (InObject) WorldDevice = InObject;
	else WorldDevice.Reset();
}

UObject* AFRPlayerBase::GetWorldDevice() const
{
	return WorldDevice.LoadSynchronous();
}

void AFRPlayerBase::AddEnemyToStack(const UObject* InObject)
{
	EnemyStack.Remove(nullptr);
	EnemyStack.Add(InObject);
	EnemyStackChanged();
}

void AFRPlayerBase::RemoveEnemyFromStack(const UObject* InObject)
{
	EnemyStack.Remove(nullptr);
	EnemyStack.Remove(InObject);
	EnemyStackChanged();
}

void AFRPlayerBase::ClearEnemyStack()
{
	EnemyStack.Empty();
	EnemyStackChanged();
}

void AFRPlayerBase::FadeToBlack(const float InTime, const bool bAudio) const
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->StartCameraFade(0.0f, 1.0f,
			InTime, FLinearColor::Black, bAudio, true);
	}
}

void AFRPlayerBase::FadeFromBlack(const float InTime, const bool bAudio) const
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f,
			InTime, FLinearColor::Black, bAudio, true);
	}
}

void AFRPlayerBase::ClearFade() const
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->StopCameraFade();
	}
}

void AFRPlayerBase::EnemyStackChanged()
{
	if (EnemyStack.IsEmpty())
	{
		RemoveFieldOfViewModifier(CHASE_FOV_KEY);
	}
	else
	{
		AddFieldOfViewModifier(CHASE_FOV_KEY, ChaseFOV);
	}
}

void AFRPlayerBase::TeleportPlayer(const FVector& InLocation, const FRotator& InRotation)
{
	SetActorLocation(InLocation, false, nullptr, ETeleportType::ResetPhysics);

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
		GetCapsuleComponent()->GetScaledCapsuleRadius() + HalfHeights.X + 5.0f, Start, End);
        		
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
	return false;
}

void AFRPlayerBase::TickStamina()
{
	const bool bShouldDrain = IsMoving() && IsRunning();
	StaminaDelta = bShouldDrain ? -StaminaRates.Y : StaminaRates.X;
	CurrentStamina = FMath::Clamp(StaminaDelta + CurrentStamina, 0.0f, MaxStamina);
	if (CurrentStamina < 0.01f && !IsStaminaPunished())
	{
		SetRunState(false);
		StateFlags |= PLAYER_STATE_FLAG(PCF_RunLocked);
	}
	else if (FMath::IsNearlyEqual(CurrentStamina, MaxStamina, 1.0f) && IsStaminaPunished())
	{
		StateFlags &= ~PLAYER_STATE_FLAG(PCF_RunLocked);
	}
}

void AFRPlayerBase::TickFootstep()
{
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

void AFRPlayerBase::SlowTick(const float DeltaTime)
{
	if (IsRunning())
	{
		if (!FieldOfView.Modifiers.Contains(RUN_FOV_KEY))
		{
			FieldOfView.Modifiers.Add(RUN_FOV_KEY, RunningFOV);
		}
	}
	else if (FieldOfView.Modifiers.Contains(RUN_FOV_KEY))
	{
		FieldOfView.Modifiers.Remove(RUN_FOV_KEY);
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Tick... Tock..."));
}

void AFRPlayerBase::BeginPlay()
{
	Super::BeginPlay();
	CameraShakes.LoadAssetsAsync();
	FootstepSounds.LoadAssetsAsync();
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	if (const APlayerCameraManager* CM = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		EquipmentRoot->AttachToComponent(CM->GetTransformComponent(),
			FAttachmentTransformRules::KeepRelativeTransform);
	}

	GameMode = FRGamemode(this);
	if (GameMode)
	{
		GameMode->PhotoModeActor = PhotoModeActor.LoadSynchronous();
		GameMode->InspectionActor = InspectionActor.LoadSynchronous();
	}

	PlayerController = FRPlayerController(this);

	FTimerManager& TM = GetWorldTimerManager();
	TM.SetTimer(StaminaTimer, this, &AFRPlayerBase::TickStamina, 0.1f, true);
	TM.SetTimer(WallDetectTimer, this, &AFRPlayerBase::LeanWallDetect, 0.1f, true);
	TM.PauseTimer(WallDetectTimer);
	if (!HasControlFlag(PCF_UseStamina))
	{
		TM.PauseTimer(StaminaTimer);
	}
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
}

void AFRPlayerBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	FootstepSounds.CheckEntries();
	ULightingDataLibrary::SetPointLightProperties(PlayerLight, PlayerLightSettings);
	
	for (const FName& ActionName : Player::Actions::All)
	{
		if (!InputActions.Contains(ActionName)) InputActions.Add(ActionName);
	}
	
	for (const AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!IsValid(Actor)) continue;
		if (Actor->IsA(APhotoModeActor::StaticClass()))
		{
			PhotoModeActor = Actor;
		}
		else if (Actor->IsA(APhotoModeActor::StaticClass()))
		{
			InspectionActor = Actor;
		}
	}
}

void AFRPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/* Statics */

AFRPlayerBase* AFRPlayerBase::GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<AFRPlayerBase> Class)
{
	AFRPlayerBase* Obj = Cast<AFRPlayerBase>(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0));
	if (!IsValid(Obj)) Obj = Cast<AFRPlayerBase>(UGameplayStatics::GetActorOfClass(WorldContextObject, StaticClass()));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}
