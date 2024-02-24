// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "GCPlayerCharacter.h"
#include "GCCameraSmoother.h"
#include "GCPlayerController.h"
#include "Framework/GCGameInstance.h"
#include "PhotoMode/GCPhotoModeActor.h"
#include "Inventory/GCInspectionActor.h"
#include "UserSettings/GCUserSettings.h"
#include "GameMusic/GCGameMusicManager.h"
#include "Inventory/GCInventoryManager.h"
#include "UserInterface/GCEyeShutWidget.h"
#include "UserInterface/Messaging/GCMessageWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "EngineUtils.h"
#include "RCMathLibrary.h"
#include "Interfaces/GCDeviceInterface.h"
#include "UserInterface/GCLoadingWidget.h"
#include "UserInterface/Gameplay/GCGameplayWidget.h"
#include "UserInterface/Gameplay/GCInventoryWidget.h"
#include "UserInterface/Gameplay/GCNarrativeWidget.h"
#if WITH_EDITOR
#include "Components/BillboardComponent.h"
#endif

#define IS_AT_STATE(State) ActiveState == EGCPlayerActiveState::State
#define NOT_AT_STATE(State) ActiveState != EGCPlayerActiveState::State
#define HAS_ABILITY(Ability) AbilityFlags & (uint8)EGCPlayerAbilityFlags::Ability

#define BIND_INPUT_ACTION(Component, Type, Event, Function) \
	if (const UInputAction* Action = InputActions.FindRef(EGCPlayerInputTypes::Type)) \
	{ \
		Component->BindAction(Action, ETriggerEvent::Event, this, &AGCPlayerCharacter::Function); \
	}

AGCPlayerCharacter::AGCPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoReceiveInput = EAutoReceiveInput::Player0;

	CameraSmoother = CreateDefaultSubobject<UGCCameraSmoother>("CameraSmoother");
	CameraSmoother->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	CameraSmoother->SetupAttachment(GetCapsuleComponent());
	
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	PlayerCamera->SetupAttachment(CameraSmoother, UGCCameraSmoother::SocketName);

	EquipmentRoot = CreateDefaultSubobject<USceneComponent>("EquipmentRoot");
	EquipmentRoot->SetupAttachment(PlayerCamera);

	FootstepAudio = CreateDefaultSubobject<UAudioComponent>("FootstepAudio");
	FootstepAudio->SetRelativeLocation(FVector(0.0f, 0.0f, -60.0f));
	FootstepAudio->SetupAttachment(GetCapsuleComponent());

#if WITH_EDITOR
	VisualIcon = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("VisualIcon");
	if (VisualIcon)
	{
		VisualIcon->SetRelativeScale3D_Direct(FVector(0.25f, 0.25f, 0.25f));
		VisualIcon->bIsScreenSizeScaled = true;
		VisualIcon->SetupAttachment(EquipmentRoot);
		const ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(TEXT("/Engine/EditorResources/S_TargetPoint.S_TargetPoint"));
		if (IconFinder.Succeeded())
		{
			VisualIcon->Sprite = IconFinder.Object;
		}
	}
#endif

#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
	bRunConstructionScriptOnDrag = true;
#endif

	StartingState = EGCPlayerActiveState::Normal;
	ActiveState = EGCPlayerActiveState::Normal;
	bCanPause = true;
	bHiding = false;
	
	bCanInteract = true;
	InteractTraceChannel = ECC_Visibility;
	ReachDistance = 250.0f;
	bCanTurn = true;
	Sensitivity = FVector2D::UnitVector;
	FieldOfView = {90.0f , false};
	LockOnTarget = nullptr;
	LockOnSpeed = 5.0f;
	bCanMove = true;
	WalkingSpeed = 300.0f;
	CameraSway = {2.5f, 1.5f};
	WalkMultiplier = {1.0f , true, 0.0f};
	bCanRun = true;
	RunningSpeed = 750.0f;
	RunningFOVModifier = 5.0f;
	MaxStamina = 100.0f;
	StaminaDrain = 1.75f;
	StaminaGain = {2.5f, 1.5f};
	bCanCrouch = true;
	CeilingTraceChannel = ECC_Visibility;
	StandingHalfHeight = 88.0f;
	CrouchingHalfHeight = 45.0f;
	CrouchWalkingSpeed = 200.0f;
	CrouchInterpSpeed = 10.0f;
	CrouchingFOVModifier = -5.0f;
	bCanLean = true;
	WallTraceChannel = ECC_Visibility;
	WallTraceLength = 125.0f;
	LeanInterpSpeed = 7.5f;
	LeanOffsets = {75.0f, 25.0f};
	bCanCloseEyes = false;
	
	PhotoModeActor = nullptr;
	InspectionActor = nullptr;
	EyeShutWidgetClass = NULL;
	CameraShakes = {};
	FootstepSounds = {};
	FootstepSounds.CheckEntries();
	for (const EGCPlayerInputTypes Type : TEnumRange<EGCPlayerInputTypes>()) 
	{ 
		InputActions.Add(Type);
	}

	EyeShutWidget = nullptr;
	ActiveWorldDevice = nullptr;
	InteractionData = {};
	EnemyStack = {};
	PlayerController = nullptr;
	
	bRunning = false;
	bCrouching = false;
	LeanState = EGCLeanState::None;
	MaxWalkSpeedTarget = WalkingSpeed;
	FieldOfViewValue = {FieldOfView.GetDefaultValue()};
	HalfHeightValue = {88.0f};
	CurrentCameraOffset = FVector2D::ZeroVector;
	TargetCameraSway = FVector2D::ZeroVector;
	TargetLeanOffset = FVector2D::ZeroVector;
	CamRelativePos = {0.0f, 0.0f, 70.0f};
	InternalFOVMod = 0.0f;
	CurrentStamina = MaxStamina;
	StaminaDelta = 0.0f;
	bStaminaPunished = false;
	bHaveEyesClosed = false;
	bShouldBeInteracting = true;
}

AGCPlayerCharacter* AGCPlayerCharacter::Get(const UObject* WorldContext)
{
	const UGCGameInstance* GI = UGCGameInstance::Get(WorldContext);
	if (AGCPlayerCharacter* FoundObj = GI ? GI->PlayerCharacter : nullptr)
		return FoundObj;

	const AGCPlayerController* PC = AGCPlayerController::Get(WorldContext);
	return PC ? PC->GetPawn<AGCPlayerCharacter>() : nullptr;
}

void AGCPlayerCharacter::SetCustomBooleanValue_Implementation(const FString& Key, const bool Value)
{
	if (Key == ValueIDs::CanRun) SetCanRun(Value);
	else if (Key == ValueIDs::CanPause) SetCanPause(Value);
}

void AGCPlayerCharacter::SetCustomNumberValue_Implementation(const FString& Key, const float Value)
{
	if (Key == ValueIDs::MaxStamina) MaxStamina = Value;
	else if (Key == ValueIDs::WalkMulti) AddWalkMultiplierModifier(TEXT("Level"), Value);
	else if (Key == ValueIDs::RunSpeed) RunningSpeed = Value;
}

bool AGCPlayerCharacter::IsInInvincibleState() const
{
	return IS_AT_STATE(Dialogue) || IS_AT_STATE(Cutscene) || IS_AT_STATE(Jumpscare) || IS_AT_STATE(Loading);
}

void AGCPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (FieldOfViewValue.IsDifferent())
	{
		PlayerCamera->SetFieldOfView(FieldOfViewValue.GetValue());
		FieldOfViewValue.Interp(DeltaTime, FieldOfView.GetInterpSpeed());
	}
	
	if (HalfHeightValue.IsDifferent())
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(HalfHeightValue.GetValue());
		HalfHeightValue.Interp(DeltaTime, CrouchInterpSpeed);
	}

	if (LockOnTarget)
	{
		SetLeanState(EGCLeanState::None);
		CameraSmoother->bIsLockOn = true;

		const FRotator Current = GetController()->GetControlRotation();
		const FRotator Target = UKismetMathLibrary::FindLookAtRotation(PlayerCamera->GetComponentLocation(),
			LockOnTarget->GetComponentLocation());
		
		GetController()->SetControlRotation(FMath::RInterpTo(Current, Target, DeltaTime, LockOnSpeed));
	}
	else
	{
		CameraSmoother->bIsLockOn = false;
		if (const FVector2D CamOffset = TargetCameraSway + TargetLeanOffset; CurrentCameraOffset != CamOffset)
		{
			CurrentCameraOffset = FMath::Vector2DInterpTo(CurrentCameraOffset, CamOffset, DeltaTime, LeanInterpSpeed);
			CamRelativePos.Y = CurrentCameraOffset.X;

			const FRotator CtrlRot = GetController()->GetControlRotation();
			GetController()->SetControlRotation(FRotator(CtrlRot.Pitch, CtrlRot.Yaw, CurrentCameraOffset.Y));
		}

		if (IsMoving() && IsPlayerControlled())
		{
			const TSubclassOf<UCameraShakeBase> Shake = bRunning ? CameraShakes.RunShake : CameraShakes.WalkShake;
			const float Scale = bRunning ? CameraShakes.RunShakeScale : CameraShakes.WalkShakeScale;

			PlayerController->ClientStartCameraShake(Shake, Scale);
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeedTarget * WalkMultiplier.GetValue();
	
	InternalFOVMod = bRunning ? RunningFOVModifier : bCrouching ? CrouchingFOVModifier : 0.0f;
	InternalFOVMod != 0.0f && IsMoving() ?
		AddFieldOfViewModifier(TEXT("InternalFOVMod"), InternalFOVMod) :
		RemoveFieldOfViewModifier(TEXT("InternalFOVMod"));

	if (!FootstepTimer.IsValid() && IsMoving() && !GetCharacterMovement()->IsFalling())
	{
		GetWorldTimerManager().SetTimer(FootstepTimer, this, &AGCPlayerCharacter::TickFootsteps,
			FootstepSounds.GetInterval(bRunning, bCrouching), false);
	}

	const float HalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight_WithoutHemisphere();
	FootstepAudio->SetRelativeLocation({0.0f, 0.0f, -HalfHeight});
	
	const float CamHeight = HalfHeight + GetCapsuleComponent()->GetUnscaledCapsuleRadius() * 0.5f;
	CamRelativePos.Z = FMath::FInterpTo(CamRelativePos.Z, CamHeight, DeltaTime, 10.0f);

	CameraSmoother->SetRelativeLocation(CamRelativePos);
}

void AGCPlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FootstepSounds.CheckEntries();
	for (const EGCPlayerInputTypes Type : TEnumRange<EGCPlayerInputTypes>()) 
	{
		if (!InputActions.Contains(Type))
		{
			InputActions.Add(Type);
		}
	}
	
	if (!PhotoModeActor)
	{
		for (AGCPhotoModeActor* Actor : TActorRange<AGCPhotoModeActor>(GetWorld()))
		{
			if (Actor)
			{
				PhotoModeActor = Actor;
				break;
			}
		}
	}

	if (!InspectionActor)
	{
		for (AGCInspectionActor* Actor : TActorRange<AGCInspectionActor>(GetWorld()))
		{
			if (Actor)
			{
				InspectionActor = Actor;
				break;
			}
		}
	}
}

void AGCPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		BIND_INPUT_ACTION(EnhancedInputComponent, Pause, Started, InputBinding_Pause);
		BIND_INPUT_ACTION(EnhancedInputComponent, Move, Triggered, InputBinding_Move);
		BIND_INPUT_ACTION(EnhancedInputComponent, Move, Completed, InputBinding_Move);
		BIND_INPUT_ACTION(EnhancedInputComponent, Move, Canceled, InputBinding_Move);
		BIND_INPUT_ACTION(EnhancedInputComponent, Turn, Triggered, InputBinding_Turn);
		BIND_INPUT_ACTION(EnhancedInputComponent, Run, Started, InputBinding_Run);
		BIND_INPUT_ACTION(EnhancedInputComponent, Run, Completed, InputBinding_Run);
		BIND_INPUT_ACTION(EnhancedInputComponent, Run, Canceled, InputBinding_Run);
		BIND_INPUT_ACTION(EnhancedInputComponent, Crouch, Started, InputBinding_Crouch);
		BIND_INPUT_ACTION(EnhancedInputComponent, Lean, Started, InputBinding_Lean);
		BIND_INPUT_ACTION(EnhancedInputComponent, Lean, Completed, InputBinding_Lean);
		BIND_INPUT_ACTION(EnhancedInputComponent, Lean, Canceled, InputBinding_Lean);
		BIND_INPUT_ACTION(EnhancedInputComponent, Inventory, Started, InputBinding_Inventory);
		BIND_INPUT_ACTION(EnhancedInputComponent, HideQuests, Started, InputBinding_HideQuests);
		BIND_INPUT_ACTION(EnhancedInputComponent, Interact, Triggered, InputBinding_Interact);
		BIND_INPUT_ACTION(EnhancedInputComponent, Interact, Completed, InputBinding_Interact);
		BIND_INPUT_ACTION(EnhancedInputComponent, Interact, Canceled, InputBinding_Interact);
		BIND_INPUT_ACTION(EnhancedInputComponent, CloseEyes, Started, InputBinding_CloseEyes);
		BIND_INPUT_ACTION(EnhancedInputComponent, CloseEyes, Completed, InputBinding_CloseEyes);
		BIND_INPUT_ACTION(EnhancedInputComponent, CloseEyes, Canceled, InputBinding_CloseEyes);
		BIND_INPUT_ACTION(EnhancedInputComponent, Equipment_Toggle, Started, InputBinding_Equipment_Toggle);
		BIND_INPUT_ACTION(EnhancedInputComponent, Equipment_Charge, Started, InputBinding_Equipment_Charge);
		BIND_INPUT_ACTION(EnhancedInputComponent, Equipment_Charge, Completed, InputBinding_Equipment_Charge);
		BIND_INPUT_ACTION(EnhancedInputComponent, Equipment_Charge, Canceled, InputBinding_Equipment_Charge);
	}
}

bool AGCPlayerCharacter::TeleportTo(const FVector& DestLocation, const FRotator& DestRotation, bool bIsATest, bool bNoCheck)
{
	//return Super::TeleportTo(DestLocation, DestRotation, bIsATest, bNoCheck);
	
	PlayerController->PlayerCameraManager->SetGameCameraCutThisFrame();
	PlayerController->SetControlRotation(DestRotation);

	CameraSmoother->OverrideWorldRotation(DestRotation);
	
	SetActorLocation(DestLocation);
	return true;
}

void AGCPlayerCharacter::InputBinding_Pause(const FInputActionValue& InValue)
{
	if (IS_AT_STATE(Device) && ActiveWorldDevice)
	{
		GCDeviceInterface::ForceExitDevice(ActiveWorldDevice, TEXT("Exiting"));
		return;
	}
	
	if (!PlayerController->IsPaused())
	{
		if (bCanPause && !LoadingWidget->IsInViewport() &&!bHaveEyesClosed && (IS_AT_STATE(Normal) || IS_AT_STATE(Inventory)))
		{
			PlayerController->PauseGame();
		}
		else
		{
			PlayerController->GetUserWidget<UGCMessageWidget>()->QueueNotice(
				FGCNoticeData{ INVTEXT("Cannot Pause at the moment"), 2.0f }, true);
		}
	}
}

void AGCPlayerCharacter::InputBinding_Move(const FInputActionValue& InValue)
{
	if (bHiding || IsGamePaused())
		return;
	
	if (IS_AT_STATE(Normal) && bCanMove)
	{
		const FVector2D Axis = InValue.Get<FVector2D>();
		if (Axis.X != 0.0f)
		{
			AddMovementInput(GetActorForwardVector(), Axis.X);
		}
		if (Axis.Y != 0.0f)
		{
			AddMovementInput(GetActorRightVector(), Axis.Y);
		}

		TargetCameraSway = CameraSway * Axis.Y;
	}
	else
	{
		TargetCameraSway = FVector2D::ZeroVector;
	}
}

void AGCPlayerCharacter::InputBinding_Turn(const FInputActionValue& InValue)
{
	if (!bHiding && !IsGamePaused() && IS_AT_STATE(Normal) && bCanTurn && !LockOnTarget)
	{
		const FVector2D Axis = InValue.Get<FVector2D>();
		if (Axis.X != 0.0f)
		{
			AddControllerYawInput(Axis.X * Sensitivity.X);
		}
		if (Axis.Y != 0.0f)
		{
			AddControllerPitchInput(Axis.Y * Sensitivity.Y * -1.0f);
		}
	}
}

void AGCPlayerCharacter::InputBinding_Run(const FInputActionValue& InValue)
{
	if (!bHiding && !IsGamePaused() && IS_AT_STATE(Normal))
	{
		SetRunState(bCanRun && !bStaminaPunished && InValue.Get<bool>());
	}
}

void AGCPlayerCharacter::InputBinding_Crouch(const FInputActionValue& InValue)
{
	if (!bHiding && !IsGamePaused() && IS_AT_STATE(Normal))
	{
		if (bCanCrouch && !bCrouching)
		{
			SetCrouchState(true);
		}
		else if (!IsStandingBlocked())
		{
			SetCrouchState(false);
		}
	}
}

void AGCPlayerCharacter::InputBinding_Lean(const FInputActionValue& InValue)
{
	if (!bHiding && !IsGamePaused() && IS_AT_STATE(Normal) && !LockOnTarget)
	{
		const float Direction = InValue.Get<float>();
		if (!bCanLean || Direction == 0.0f)
		{
			SetLeanState(EGCLeanState::None);
		}
		else if (Direction > 0.0f)
		{
			SetLeanState(EGCLeanState::Right);
		}
		else // if (Direction < 0.0f)
		{
			SetLeanState(EGCLeanState::Left);
		}
	}
}

void AGCPlayerCharacter::InputBinding_Inventory(const FInputActionValue& InValue)
{
	if (!bHiding && !bHaveEyesClosed && !IsGamePaused())
	{
		if (IS_AT_STATE(Normal))
		{
			PlayerController->OpenInventory();	
		}
		else if (IS_AT_STATE(Inventory))
		{
			PlayerController->CloseInventory();
		}
	}
}

void AGCPlayerCharacter::InputBinding_HideQuests(const FInputActionValue& InValue)
{
	PlayerController->ToggleQuestsHidden();
}

void AGCPlayerCharacter::InputBinding_Interact(const FInputActionValue& InValue)
{
	if (!bHiding && !bHaveEyesClosed && !IsGamePaused() && IS_AT_STATE(Normal) && bCanInteract && InValue.Get<bool>()) // Is Interacting
	{
		if (bShouldBeInteracting)
		{
			FHitResult HitResult;
			FGCInteractionData NewData;
			if (TraceInteraction(HitResult, NewData))
			{
				if (NewData != InteractionData)
				{
					GCInteraction::OnEndInteract(InteractionData.Actor, this);
					GCInteraction::OnBeginInteract(NewData.Actor, this, HitResult);
					InteractionData = NewData;
				}

				return;
			}
			
			bShouldBeInteracting = false;
		}
	}
	else
	{
		bShouldBeInteracting = true;
	}

	GCInteraction::OnEndInteract(InteractionData.Actor, this);
	InteractionData.Reset();
}

void AGCPlayerCharacter::InputBinding_CloseEyes(const FInputActionValue& InValue)
{
	if (!bHiding && IS_AT_STATE(Normal))
	{
		SetEyesCloseState(bCanCloseEyes && InValue.Get<bool>());
	}
}

void AGCPlayerCharacter::InputBinding_Equipment_Toggle(const FInputActionValue& InValue)
{
	if (!bHiding && !IsGamePaused() && IS_AT_STATE(Normal))
	{
		if (InValue.Get<bool>())
		{
			PlayerController->GetInventoryManager()->ToggleEquipment();
		}
	}
}

void AGCPlayerCharacter::InputBinding_Equipment_Charge(const FInputActionValue& InValue)
{
	if (!bHiding && !IsGamePaused() && IS_AT_STATE(Normal))
	{
		PlayerController->GetInventoryManager()->SetEquipmentCharging(InValue.Get<bool>());
	}
}

void AGCPlayerCharacter::SetRunState(const bool bNewState)
{
	if (bRunning != bNewState)
	{
		bRunning = !bCrouching ? bNewState : false;
		if (!bCrouching)
		{
			MaxWalkSpeedTarget = bRunning ? RunningSpeed : WalkingSpeed;
		}
	}
}

void AGCPlayerCharacter::SetCrouchState(const bool bNewState)
{
	if (bCrouching != bNewState)
	{
		bCrouching = !bRunning ? bNewState : false;
		HalfHeightValue.SetTarget(bCrouching ? CrouchingHalfHeight : StandingHalfHeight);
		if (!bRunning)
		{
			MaxWalkSpeedTarget = bCrouching ? CrouchWalkingSpeed : WalkingSpeed;
		}
	}
}

void AGCPlayerCharacter::SetLeanState(const EGCLeanState NewState)
{
	if (LeanState != NewState)
	{
		if (NewState == EGCLeanState::None)
		{
			LeanState = EGCLeanState::None;
			TargetLeanOffset = FVector2D::ZeroVector;
			GetWorldTimerManager().PauseTimer(WallDetectionTimer);
		}
		else
		{
			float Direction = 0.0f;
			switch (NewState)
			{
			case EGCLeanState::Left:
				Direction = -1.0f;
				break;

			case EGCLeanState::Right:
				Direction = 1.0f;
				break;

			default: break;
			}

			check(Direction != 0.0f);
			if (!IsLeaningBlocked(Direction))
			{
				LeanState = NewState;
				TargetLeanOffset = LeanOffsets * Direction;
				GetWorldTimerManager().UnPauseTimer(WallDetectionTimer);
			}
		}
	}
}

void AGCPlayerCharacter::SetEyesCloseState(const bool bNewState)
{
	if (bHaveEyesClosed != bNewState)
	{
		bHaveEyesClosed = bNewState;
		EyeShutWidget->SetShutEyesState(bHaveEyesClosed);
		bHaveEyesClosed ? OnEyesClosed.Broadcast() : OnEyesOpened.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Eyes closed %s"), *LexToString(bHaveEyesClosed));
	}
}

void AGCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UGCGameInstance::Get(this)->PlayerCharacter = this;
	PlayerController = AGCPlayerController::Get(this);
	PlayerController->PlayerCameraManager->SetManualCameraFade(
		1.0f, FColor::Black, true);
	
	SetActiveState(StartingState);
	
	const bool bCachedCanPause = bCanPause;
	bCanPause = false;
	
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, [WEAK_THIS, bCachedCanPause](){
		if (!WeakThis.IsValid()) return;
		
		WeakThis->bCanPause = bCachedCanPause;
		if (WeakThis->LoadingWidget && WeakThis->LoadingWidget->IsInViewport())
		{
			WeakThis->PlayerController->PlayerCameraManager->StartCameraFade(
				1.0f, 0.0f, 1.0f, FColor::Black, true);
		}
	}, 1.0f, false);
	
	EquipmentRoot->AttachToComponent(PlayerController->PlayerCameraManager->GetTransformComponent(),
	   FAttachmentTransformRules::KeepRelativeTransform);

	EyeShutWidget = CreateWidget<UGCEyeShutWidget>(PlayerController, EyeShutWidgetClass);
	EyeShutWidget->AddToViewport();
	
	if (UGCUserSettings* Settings = UGCUserSettings::Get())
	{
		Settings->OnDynamicApply.AddUObject(this, &AGCPlayerCharacter::OnDynamicSettingsApply);
		Settings->OnManualApply.AddUObject(this, &AGCPlayerCharacter::OnManualSettingsApply);
		OnDynamicSettingsApply(Settings);
		OnManualSettingsApply(Settings);
	}

	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;

	GetWorldTimerManager().SetTimer(StaminaChangeTimer, this, &AGCPlayerCharacter::TickStamina, 0.1f, true);
	GetWorldTimerManager().SetTimer(WallDetectionTimer, this, &AGCPlayerCharacter::LeanWallDetection, 0.1f, true);
	GetWorldTimerManager().PauseTimer(WallDetectionTimer);

	GetWorldTimerManager().SetTimer(WindowFocusTimer, this, &AGCPlayerCharacter::TickWindowFocus, 0.5f, true);
	GetWorldTimerManager().PauseTimer(WindowFocusTimer);

	FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &AGCPlayerCharacter::OnWindowFocusChanged);
}

void AGCPlayerCharacter::OnDynamicSettingsApply(UGCUserSettings* InUserSettings)
{
	SetFieldOfView(InUserSettings->GetFieldOfView());
	
	FieldOfViewValue.SnapToTarget();
	PlayerCamera->SetFieldOfView(FieldOfViewValue.GetValue());
}

void AGCPlayerCharacter::OnManualSettingsApply(UGCUserSettings* InUserSettings)
{
	FVector2D SensitivityXY = InUserSettings->GetSensitivityXY();
	if (InUserSettings->GetInvertMouseX()) { SensitivityXY.X *= -1.0f; }
	if (InUserSettings->GetInvertMouseY()) { SensitivityXY.Y *= -1.0f; }
	SetSensitivity(SensitivityXY);
	
	CameraSmoother->SetSmootherEnabled(InUserSettings->GetEnableCameraSmoothing());
	PlayerCamera->bUsePawnControlRotation = !InUserSettings->GetEnableCameraSmoothing();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AGCPlayerCharacter::OnWindowFocusChanged(const bool bFocused)
{
	if (InputEnabled() && GetController())
	{
		if (bFocused)
		{
			GetWorldTimerManager().PauseTimer(WindowFocusTimer);
		}
		else
		{
			GetWorldTimerManager().UnPauseTimer(WindowFocusTimer);
		}
	}
}

bool AGCPlayerCharacter::TraceInteraction(FHitResult& OutHitResult, FGCInteractionData& OutData) const
{
	OutData.Reset();
	OutHitResult = {};

	if (bHiding) return false;
	
	FVector Start, End = FVector::ZeroVector;
	URCMathLibrary::GetCameraLineTraceVectors(this, ERCVectorDirection::Forward, ReachDistance, Start, End);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, InteractTraceChannel,
		FCollisionQueryParams(TEXT("GCTrace_Interaction"), false, this)))
	{
		FText DisplayName;
		if (GCInteraction::GetInteractionInfo(HitResult.GetActor(), DisplayName))
		{
			OutHitResult = HitResult;
			OutData.DisplayName = DisplayName;
			OutData.Actor = HitResult.GetActor();
			return true;
		}
	}

	return false;
}

bool AGCPlayerCharacter::IsLeaningBlocked(const float Direction) const
{
	if (FMath::IsNearlyZero(Direction))
		return false;
	
	FVector Start, End = FVector::ZeroVector;
	URCMathLibrary::GetActorLineTraceVectors(this, ERCVectorDirection::Right, WallTraceLength * Direction, Start, End);
	Start += FVector(0.0f, 0.0f, CameraSmoother->GetRelativeLocation().Z);
	End += FVector(0.0f, 0.0f, CameraSmoother->GetRelativeLocation().Z);

	FHitResult HitResult;
	return GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, WallTraceChannel,
		FCollisionShape::MakeSphere(10.0f), FCollisionQueryParams(TEXT("GCTrace_Lean"), false, this));
}

bool AGCPlayerCharacter::IsStandingBlocked() const
{
	FVector Start, End = FVector::ZeroVector;
	URCMathLibrary::GetActorLineTraceVectors(this, ERCVectorDirection::Up,
		GetCapsuleComponent()->GetUnscaledCapsuleRadius() + StandingHalfHeight + UE_KINDA_SMALL_NUMBER, Start, End);
        		
	FHitResult HitResult;
	return GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, CeilingTraceChannel,
		FCollisionShape::MakeSphere(10.0f), FCollisionQueryParams(TEXT("GCTrace_Stand"), false, this));
}

void AGCPlayerCharacter::TickWindowFocus()
{
	if (!IsGamePaused())
	{
		InputBinding_Pause({});
	}
}

void AGCPlayerCharacter::TickStamina()
{
	const bool bShouldDrain = IsMoving() && bRunning && !GetCharacterMovement()->IsFalling();
	StaminaDelta = bShouldDrain ? -StaminaDrain : IsMoving() ? StaminaGain.Y : StaminaGain.X;
	CurrentStamina = FMath::Clamp(StaminaDelta + CurrentStamina, 0.0f, MaxStamina);
	if (CurrentStamina < 1.0f && !bStaminaPunished)
	{
		SetRunState(false);
		bStaminaPunished = true;
	}
	else if (FMath::IsNearlyEqual(CurrentStamina, MaxStamina, 5.0f) && bStaminaPunished)
	{
		bStaminaPunished = false;
	}
}

void AGCPlayerCharacter::LeanWallDetection()
{
	float Direction = 0.0f;
	switch (LeanState)
	{
	case EGCLeanState::Left:
		Direction = -1.0f;
		break;

	case EGCLeanState::Right:
		Direction = 1.0f;
		break;

	default: break;
	}

	if (FMath::IsNearlyZero(Direction))
	{
		GetWorldTimerManager().PauseTimer(WallDetectionTimer);
		return;
	}
	
	if (IsLeaningBlocked(Direction))
	{
		SetLeanState(EGCLeanState::None);
	}
}

void AGCPlayerCharacter::TickFootsteps()
{
	FVector Start, End = FVector::ZeroVector;
	URCMathLibrary::GetComponentLineTraceVectors(FootstepAudio, ERCVectorDirection::Up, -100.0f , Start, End);

	FCollisionQueryParams Params{TEXT("GCTrace_Footsteps"), true, this};
	Params.bReturnPhysicalMaterial = true;
		
	FHitResult HitResult;
#if WITH_EDITOR
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, FootstepSounds.FloorTraceChannel, Params))
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
#else
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, FootstepSounds.FloorTraceChannel, Params);
#endif
	if (GetVelocity().Size() > 25.0f)
	{
		const EPhysicalSurface Surface = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
		if (USoundBase* Sound = FootstepSounds.GetAudio(bRunning, bCrouching, Surface))
		{
			FootstepAudio->SetVolumeMultiplier(FootstepSounds.Volume);
			FootstepAudio->SetSound(Sound);
			FootstepAudio->Play();
		}
	}
	
	FootstepTimer.Invalidate();
}

bool AGCPlayerCharacter::IsOnTaskOrDeviceActor() const
{
	return GCInteraction::IsTaskActor(InteractionData.Actor) || ActiveWorldDevice;
}

bool AGCPlayerCharacter::InMovementAction() const
{
	return !bHiding && !IsInInvincibleState()
	&& (IsMoving() || IsOnTaskOrDeviceActor() || IS_AT_STATE(Device));
}

void AGCPlayerCharacter::ResetStates()
{
	SetRunState(false);
	SetCrouchState(false);
	SetLeanState(EGCLeanState::None);
	SetStaminaPercent(1.0f);

	SetEyesCloseState(false);
}

void AGCPlayerCharacter::SetHiding(const bool bInIsHiding)
{
	if (bHiding != bInIsHiding)
	{
		bHiding = bInIsHiding;
		SetActorHiddenInGame(bHiding);
		if (PlayerController)
		{
			if (const UGCInventoryManager* Inventory = PlayerController->GetInventoryManager())
			{
				if (AActor* EquippedActor = Inventory->GetActiveEquipment().Actor)
				{
					EquippedActor->SetActorHiddenInGame(bHiding);
				}
			}
		}
		if (bHiding)
		{
			ResetStates();
		}
	}
}

void AGCPlayerCharacter::SetStaminaPercent(const float InPercent)
{
	CurrentStamina = MaxStamina * FMath::Clamp(InPercent, 0.0f, 1.0f);
}

void AGCPlayerCharacter::SetActiveState(const EGCPlayerActiveState InState)
{
	if (ActiveState != InState)
	{
		ActiveState = InState;
		if (NOT_AT_STATE(Normal))
		{
			SetRunState(false);
			SetCrouchState(false);
			SetLeanState(EGCLeanState::None);
			SetEyesCloseState(false);
			SetStaminaPercent(1.0f);
		}

		OnStateChanged.Broadcast(ActiveState);
		GetGameInstance<UGCGameInstance>()->UpdatePlayerInvincible();
	}
}

void AGCPlayerCharacter::SetCanCloseEyes(const bool InCanCloseEyes)
{
	if (bCanCloseEyes != InCanCloseEyes)
	{
		bCanCloseEyes = InCanCloseEyes;
		if (!bCanCloseEyes)
		{
			SetEyesCloseState(false);
		}
	}
}

void AGCPlayerCharacter::SetCanInteract(const bool bInValue)
{
	bCanInteract = bInValue;
}

void AGCPlayerCharacter::SetCanTurn(const bool bInValue)
{
	bCanTurn = bInValue;
}

void AGCPlayerCharacter::SetSensitivity(const FVector2D InValue)
{
	Sensitivity = InValue;
}

void AGCPlayerCharacter::SetCanMove(const bool bInValue)
{
	bCanMove = bInValue;
}

void AGCPlayerCharacter::SetCanRun(const bool bInValue)
{
	bCanRun = bInValue;
	if (!bCanRun && bRunning)
	{
		SetRunState(false);
	}
}

void AGCPlayerCharacter::SetCanCrouch(const bool bInValue)
{
	bCanCrouch = bInValue;
	if (!bCanCrouch && bCrouching && !IsStandingBlocked())
	{
		SetCrouchState(false);
	}
}

void AGCPlayerCharacter::SetCanLean(const bool bInValue)
{
	bCanLean = bInValue;
	if (!bCanLean && LeanState != EGCLeanState::None)
	{
		SetLeanState(EGCLeanState::None);
	}
}

void AGCPlayerCharacter::SetFieldOfView(const float InValue)
{
	FieldOfView.SetDefaultValue(InValue);
	FieldOfViewValue.SetTarget(FieldOfView.GetValue());
}

void AGCPlayerCharacter::AddFieldOfViewModifier(const FName InKey, const float InValue)
{
	FieldOfView.AddModifier(InKey, InValue);
	FieldOfViewValue.SetTarget(FieldOfView.GetValue());
}

void AGCPlayerCharacter::RemoveFieldOfViewModifier(const FName InKey)
{
	FieldOfView.RemoveModifier(InKey);
	FieldOfViewValue.SetTarget(FieldOfView.GetValue());
}

void AGCPlayerCharacter::SetWalkMultiplier(const float InValue)
{
	WalkMultiplier.SetDefaultValue(InValue);
}

void AGCPlayerCharacter::AddWalkMultiplierModifier(const FName InKey, const float InValue)
{
	WalkMultiplier.AddModifier(InKey, InValue);
}

void AGCPlayerCharacter::RemoveWalkMultiplierModifier(const FName InKey)
{
	WalkMultiplier.RemoveModifier(InKey);
}

void AGCPlayerCharacter::ResetPlayer()
{
	ActiveState = EGCPlayerActiveState::Normal;
	bCanPause = true;

	WalkMultiplier.ClearModifiers();
	FieldOfView.ClearModifiers();
	
	FieldOfViewValue.SetTarget(FieldOfView.GetValue());
	TargetCameraSway = FVector2D::ZeroVector;
	
	SetCanInteract(true);
	SetCanTurn(true);
	SetCanMove(true);
	SetCanRun(true);
	SetCanCrouch(true);
	SetCanLean(true);
	SetCanCloseEyes(false);
	
	SetRunState(false);
	SetCrouchState(false);
	SetLeanState(EGCLeanState::None);
	SetStaminaPercent(1.0f);
	SetEyesCloseState(false);
}

void AGCPlayerCharacter::SetWorldDevice(AActor* InActor)
{
	if (ActiveWorldDevice != InActor && (GCDeviceInterface::ImplementedBy(InActor) || !InActor))
	{
		ActiveWorldDevice = InActor;
		if (ActiveWorldDevice)
		{
			PlayerController->GetUserWidget(UGCGameplayWidget::StaticClass())->SetWidgetHidden(true);
			SetActiveState(EGCPlayerActiveState::Device);
		}
		else if (NOT_AT_STATE(Loading)) 
		{
			PlayerController->GetUserWidget(UGCGameplayWidget::StaticClass())->SetWidgetHidden(false);
			SetActiveState(EGCPlayerActiveState::Normal);
		}
	}
}

void AGCPlayerCharacter::AddEnemy(AActor* InActor)
{
	EnemyStack.AddUnique(InActor);
	EnemyStack.Remove(nullptr);
	
	if (!EnemyStack.IsEmpty())
	{
		if (UGCGameMusicManager* GameMusicManager = UGCGameMusicManager::Get(this))
		{
			GameMusicManager->SetChaseMode(true);
		}
	}
}

void AGCPlayerCharacter::RemoveEnemy(AActor* InActor)
{
	EnemyStack.Remove(InActor);
	EnemyStack.Remove(nullptr);

	if (EnemyStack.IsEmpty())
	{
		if (UGCGameMusicManager* GameMusicManager = UGCGameMusicManager::Get(this))
		{
			GameMusicManager->SetChaseMode(false);
		}
	}
}

const TArray<AActor*>& AGCPlayerCharacter::GetEnemyStack()
{
	EnemyStack.Remove(nullptr);
	return EnemyStack;
}

bool AGCPlayerCharacter::TryJumpscare()
{
	if (IS_AT_STATE(Normal) || IS_AT_STATE(Device) || IS_AT_STATE(Inventory))
	{
		if (ActiveWorldDevice) GCDeviceInterface::ForceExitDevice(ActiveWorldDevice, TEXT("Jumpscare"));
		PlayerController->ExitInventoryInternal(nullptr, true);
		PlayerController->GetUserWidget<UGCInventoryWidget>()->SetVisibility(ESlateVisibility::Collapsed);
		PlayerController->GetUserWidget<UGCGameplayWidget>()->SetVisibility(ESlateVisibility::Collapsed);
		PlayerController->GetUserWidget<UGCMessageWidget>()->SetVisibility(ESlateVisibility::Collapsed);
		PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetVisibility(ESlateVisibility::Collapsed);
		SetActiveState(EGCPlayerActiveState::Jumpscare);
		return true;
	}

	return false;
}
