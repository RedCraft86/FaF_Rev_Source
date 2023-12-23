// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "CCTVMonitor.h"
#include "Core/CoreMacros.h"
#include "RCRuntimeLibrary.h"
#include "Player/GCPlayerCharacter.h"
#include "Player/GCPlayerController.h"
#include "Interfaces/GCCharacterInterface.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EngineUtils.h"
#if WITH_EDITOR
#include "Components/SphereComponent.h"
#endif

ACCTVMonitor::ACCTVMonitor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

#if WITH_EDITOR
	InnerSphere = CreateEditorOnlyDefaultSubobject<USphereComponent>("InnerSphere");
	if (InnerSphere)
	{
		InnerSphere->ShapeColor = FColor::Yellow;
		InnerSphere->SetupAttachment(GetRootComponent());
	}
	OuterSphere = CreateEditorOnlyDefaultSubobject<USphereComponent>("OuterSphere");
	if (OuterSphere)
	{
		OuterSphere->ShapeColor = FColor::Red;
		OuterSphere->SetupAttachment(GetRootComponent());
	}
#endif

	bActive = true;
	Range = {200.0f, 800.0f};
	Resolution = {2048.0f, 2048.0f};
	Cameras = {};
	ScreenMaterial = nullptr;
	ExitAction = nullptr;
	TurnAction = nullptr;
	TurnSpeed = 10.0f;
	Player = nullptr;
	MID = nullptr;
	CVT = nullptr;

	Static1InterpSpeed = 5.0f;
	Static1.SetTarget(0.0f);
	Static1.SnapToTarget();

	Static2InterpSpeed = 5.0f;
	Static2.SetTarget(0.0f);
	Static2.SnapToTarget();
}

void ACCTVMonitor::RefreshCamera(const float WaitTime)
{
	GetWorldTimerManager().ClearTimer(Refresh);
	Static1.SetTarget(1.0f);
	Static1InterpSpeed = 5.0f;

	OnRefresh.Broadcast(false);
	OnRefreshBP.Broadcast(false);

	GetWorldTimerManager().SetTimer(Refresh, [WEAK_THIS]()
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->Static1.SetTarget(0.0f);
		WeakThis->Static1InterpSpeed = 2.0f;

		WeakThis->OnRefresh.Broadcast(true);
		WeakThis->OnRefreshBP.Broadcast(true);
	}, WaitTime, false);
}

void ACCTVMonitor::ChangeCamera(const uint8 ToIndex)
{
	if (!Cameras.IsValidIndex(ToIndex)) return;

	GetWorldTimerManager().ClearTimer(CamChange);
	Static2.SetTarget(1.0f);
	Static2InterpSpeed = 5.0f;

	GetWorldTimerManager().SetTimer(CamChange, [WEAK_THIS, ToIndex]()
	{
		if (!WeakThis.IsValid()) return;
		if (WeakThis->ActiveCamera.IsValid())
		{
			WeakThis->ActiveCamera->SetState(false);
		}

		const int32 Idx = FMath::Clamp(ToIndex, 0, WeakThis->Cameras.Num() - 1);
		WeakThis->ActiveCamera = WeakThis->Cameras[Idx];
		checkf(WeakThis->ActiveCamera.IsValid(), TEXT("This CCTV just decided to not exist anymore."))

		WeakThis->ActiveCamera->SetState(true);

		WeakThis->Static2.SetTarget(0.0f);
		WeakThis->Static2InterpSpeed = 2.0f;

		WeakThis->OnCamChange.Broadcast(Idx, WeakThis->ActiveCamera.Get());
		WeakThis->OnCamChangeBP.Broadcast(Idx, WeakThis->ActiveCamera.Get());
	}, 0.1f, false);
}

void ACCTVMonitor::DelayedTick()
{
	if (!ActiveCamera.IsValid()) return;
	if (!URCRuntimeLibrary::IsActorInScreen(this))
	{
		Static2.SetTarget(1.0f);
		ActiveCamera->SetState(false);
		return;
	}
	
	if (const APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		if (!URCRuntimeLibrary::IsLocationInFront(this, PCM->GetCameraLocation()))
		{
			Static2.SetTarget(1.0f);
			ActiveCamera->SetState(false);
			return;
		}
		
		ActiveCamera->SetState(true);
		const float Dist = FVector::Dist(PCM->GetCameraLocation(), GetActorLocation());
		const float Alpha = FMath::GetMappedRangeValueClamped(
			FVector2D{Range.GetMin(), Range.GetMax()},
			FVector2D{0.0f, 1.0f}, Dist);
		
		const FVector2D ResizeTarget = FMath::Lerp(Resolution, {128.0f, 128.0f}, Alpha);
		CVT->ResizeTarget(FMath::RoundToInt(ResizeTarget.X), FMath::RoundToInt(ResizeTarget.Y));
	}

	for (const TSoftObjectPtr<APawn>& Pawn : Pawns)
	{
		if (!Pawn.IsValid()) continue;
		GCCharacter::RunNamedEvent(Pawn.Get(),
			ActiveCamera->IsActorOnCamera(Pawn.Get()) ? TEXT("EnableStencil") : TEXT("DisableStencil"));
	}
}

void ACCTVMonitor::UpdateRenderTarget()
{
	if (!Cameras.IsEmpty() && !ActiveCamera.IsValid())
	{
		ActiveCamera = Cameras[0];
	}

	if (!CVT)
	{
		CVT = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this,
			UCanvasRenderTarget2D::StaticClass(), Resolution.X, Resolution.Y);
	}
	if (CVT && ActiveCamera.IsValid())
	{
		ActiveCamera->SceneCapture->TextureTarget = CVT;
	}

	if (!MID)
	{
		MID = UMaterialInstanceDynamic::Create(ScreenMaterial, this);
	}
	if (MID && CVT)
	{
		MID->SetTextureParameterValue(TEXT("RT"), CVT);
		MID->SetScalarParameterValue(TEXT("Static1"), 0.0f);
		MID->SetScalarParameterValue(TEXT("Static2"), 0.0f);
		
		uint8 Index = 0;
		if (UStaticMeshComponent* MeshComp = GetScreenMesh(Index))
		{
			MeshComp->SetMaterial(Index, MID);
		}
	}
}

void ACCTVMonitor::InputBinding_Exit(const FInputActionValue& InValue)
{
	if (Player)
	{
		Player->SetWorldDevice(nullptr);
		DisableInput(Player->GetPC());
		Player->GetPC()->SetViewTargetWithBlend(Player, 0.25f);

		Player = nullptr;
	}
}

void ACCTVMonitor::InputBinding_Turn(const FInputActionValue& InValue)
{
	const FVector2D Speed = InValue.Get<FVector2D>() * TurnSpeed;
	if (ActiveCamera.IsValid()) ActiveCamera->AddTurnInput(Speed.X, Speed.Y);
}

void ACCTVMonitor::BeginPlay()
{
	Super::BeginPlay();
	
	Cameras.Remove(nullptr);
	for (const APawn* Pawn : TActorRange<APawn>(GetWorld()))
	{
		if (IsValid(Pawn) && GCCharacter::IsAnimatronic(Pawn))
		{
			Pawns.Add(Pawn);
		}
	}

	CreateInputComponent(UEnhancedInputComponent::StaticClass());
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(ExitAction, ETriggerEvent::Triggered, this, &ACCTVMonitor::InputBinding_Exit);
		EIC->BindAction(TurnAction, ETriggerEvent::Ongoing, this, &ACCTVMonitor::InputBinding_Turn);
	}

	UpdateRenderTarget();
	GetWorldTimerManager().SetTimer(SlowTick, this, &ACCTVMonitor::DelayedTick, 0.1f, true, 0.0f);
}

void ACCTVMonitor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Static1.Interp(DeltaSeconds, Static1InterpSpeed);
	MID->SetScalarParameterValue(TEXT("Static1"), Static1.GetValueClamped());

	Static2.Interp(DeltaSeconds, Static2InterpSpeed);
	MID->SetScalarParameterValue(TEXT("Static2"), Static2.GetValueClamped());
}

void ACCTVMonitor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateRenderTarget();
#if WITH_EDITOR
	InnerSphere->SetSphereRadius(Range.GetMin());
	OuterSphere->SetSphereRadius(Range.GetMax());
#endif
}

void ACCTVMonitor::ForceExitDevice_Implementation(const FString& Reason)
{
	InputBinding_Exit({});
}

bool ACCTVMonitor::GetInteractionInfo_Implementation(FText& DisplayName)
{
	DisplayName = FText::FromString(TEXT("Cameras"));
	return bActive;
}

void ACCTVMonitor::OnBeginInteract_Implementation(AGCPlayerCharacter* InPlayer, const FHitResult& HitResult)
{
	Player = InPlayer;
	if (Player)
	{
		Player->SetWorldDevice(this);
		Player->GetPC()->SetViewTargetWithBlend(this, 0.25f);

		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, [WEAK_THIS]()
		{
			if (WeakThis.IsValid()) WeakThis->EnableInput(WeakThis->Player->GetPC());	
		}, 0.25f, false);
	}
}
