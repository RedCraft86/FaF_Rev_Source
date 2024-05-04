// Copyright (C) RedCraft86. All Rights Reserved.
// ReSharper disable CppParameterMayBeConst

#include "FRPlayer.h"
#include "FRGameMode.h"
#include "FRController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

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

	ControlFlags = DEFAULT_PLAYER_CONTROL_FLAGS;
	InteractTraceChannel = ECC_Visibility;
	ReachDistance = 250.0f;
	Sensitivity = FVector2D::UnitVector;
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
	LeanState = EPlayerLeanState::None;
	FieldOfViewValue = { FieldOfView.Evaluate() };
	HalfHeightValue = { FieldOfView.Evaluate() };
	InteractData = {};
	WorldDevice = nullptr;
	EnemyStack = {};
	LeanOffset = FVector2D::ZeroVector;
	SwayOffset = FVector2D::ZeroVector;
	CamOffset = FVector2D::ZeroVector;
	WalkSpeedTarget = WalkingSpeed;
	CurrentStamina = MaxStamina;
	StaminaDelta = 0.0f;
	bRunning = false;
	bCrouching = false;
	bStaminaPunished = false;
	bInteracting = false;
}

void AFRPlayerBase::ResetStates()
{
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
	}
}

void AFRPlayerBase::UnsetControlFlag(const TEnumAsByte<EPlayerControlFlags> InFlag)
{
	if (HasControlFlag(InFlag))
	{
		ControlFlags &= ~InFlag.GetValue();
	}
}

bool AFRPlayerBase::HasControlFlag(const TEnumAsByte<EPlayerControlFlags> InFlag) const
{
	return ControlFlags & InFlag.GetValue();
}

void AFRPlayerBase::AddLockFlag(const FPlayerLockState InFlag)
{
	AddLockFlag(InFlag.SelectedValue);
}

void AFRPlayerBase::AddLockFlag(const FName InFlag)
{
	LockFlags.Remove(NAME_None);
 	if (!InFlag.IsNone()) LockFlags.Add(InFlag);
}

void AFRPlayerBase::ClearLockFlag(const FPlayerLockState InFlag)
{
	ClearLockFlag(InFlag.SelectedValue);
}

void AFRPlayerBase::ClearLockFlag(const FName InFlag)
{
	LockFlags.Remove(NAME_None);
 	if (!InFlag.IsNone()) LockFlags.Remove(InFlag);
}

bool AFRPlayerBase::HasLockFlag(const FPlayerLockState InFlag) const
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

void AFRPlayerBase::TeleportPlayer(const FVector InLocation, const FRotator InRotation)
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

void AFRPlayerBase::BeginPlay()
{
	Super::BeginPlay();
	CameraShakes.LoadAssetsAsync();
	FootstepSounds.LoadAssetsAsync();
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

	PlayerController = FRController(this);
}

void AFRPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
