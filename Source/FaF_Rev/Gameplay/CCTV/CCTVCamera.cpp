// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVCamera.h"
#include "CCTVMonitor.h"
#include "CharacterAI/FREnemyBase.h"
#include "Components/AudioComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/VisionConeComponent.h"
#include "Optimization/SmartCulling.h"
#if WITH_EDITOR
#include "EngineUtils.h"
#include "Components/DebugShapesComponent.h"
#endif

ACCTVCamera::ACCTVCamera() : TurningRate(FVector2D::UnitVector), TurningRange(25.0f, 10.0f)
	, CaptureFrequency(0.25f), RefreshFrequency(0.25f)
{
	PrimaryActorTick.bCanEverTick = false;

	CaptureView = CreateDefaultSubobject<USceneComponent>("CaptureView");
	CaptureView->SetupAttachment(SceneRoot);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
	SceneCapture->SetupAttachment(CaptureView);
	SceneCapture->CaptureSource = SCS_FinalColorHDR;
	SceneCapture->bAlwaysPersistRenderingState = true;
	SceneCapture->bCaptureEveryFrame = false;
	
	VisionCone = CreateDefaultSubobject<UVisionConeComponent>("VisionCone");
	VisionCone->SetupAttachment(CaptureView);
	VisionCone->PeripheralAngle = 0.0f;
	VisionCone->BaseAngle = 45.0f;

	bEnabled = false;
	bStartWithCollisionEnabled = false;

#if WITH_EDITORONLY_DATA
	DebugShapes = CreateEditorOnlyDefaultSubobject<UDebugShapesComponent>("DebugShapes");
#endif
}

#if WITH_EDITOR
void ACCTVCamera::FindEnemies()
{
#if WITH_EDITORONLY_DATA
	if (EnemyFilterTag.IsNone()) return;
	for (const AFREnemyBase* Enemy : TActorRange<AFREnemyBase>(GetWorld()))
	{
		if (Enemy && Enemy->ActorHasTag(EnemyFilterTag)) Enemies.Add(Enemy);
	}
#endif
}
#endif

void ACCTVCamera::CaptureScene() const
{
	SceneCapture->CaptureScene();
}

void ACCTVCamera::RefreshEnemyState()
{
	for (const TObjectPtr<const AFREnemyBase>& Enemy : Enemies)
	{
		if (!Enemy) continue;
		bSeeEnemies = VisionCone->GetActorVisionState(Enemy, true) != EVisionConeState::None;
		if (bSeeEnemies) break;
	}
	
	SceneCapture->AddOrUpdateBlendable(TrackingPostProcess, bSeeEnemies ? 1.0f : 0.0f);
}

void ACCTVCamera::SetMonitor(ACCTVMonitor* InMonitor)
{
	if (InMonitor)
	{
		Monitor = InMonitor;
		SceneCapture->TextureTarget = Monitor->CaptureRT;
	}
}

void ACCTVCamera::BeginPlay()
{
	Super::BeginPlay();
	SceneCapture->AddOrUpdateBlendable(TrackingPostProcess, 0.0f);
	
	GetWorldTimerManager().SetTimer(CaptureHandle, this, &ACCTVCamera::CaptureScene, CaptureFrequency, true);
	GetWorldTimerManager().PauseTimer(CaptureHandle);
	
	GetWorldTimerManager().SetTimer(RefreshHandle, this, &ACCTVCamera::RefreshEnemyState, RefreshFrequency, true);
	GetWorldTimerManager().PauseTimer(RefreshHandle);
	
	// Enemies.Remove(nullptr);
	// for (const TObjectPtr<const AFREnemyBase>& Enemy : Enemies)
	// {
	// 	if (!Enemy) continue;
	// 	const_cast<AFREnemyBase*>(Enemy.Get())->OnAudioPlayed.AddDynamic(this, &ACCTVCamera::OnEnemyAudioPlayed);
	// }
}

void ACCTVCamera::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	VisionCone->PeripheralAngle = 0.0f;
	SceneCapture->SetRelativeRotation(FRotator::ZeroRotator);
#if WITH_EDITORONLY_DATA
	if (DebugShapes)
	{
		FDebugArcData& DebugArcX = DebugShapes->DebugArcs.FindOrAdd("ArcX");
		DebugArcX.Location = CaptureView->GetRelativeLocation();
		DebugArcX.Rotation = CaptureView->GetRelativeRotation();
		DebugArcX.MinAngle = -TurningRange.X;
		DebugArcX.MaxAngle = TurningRange.X;
		DebugArcX.Color = FColor::Green;
		DebugArcX.Thickness = 0.5f;
		DebugArcX.Sections = 16.0f;
		DebugArcX.Radius = 24.0f;

		FDebugArcData& DebugArcY = DebugShapes->DebugArcs.FindOrAdd("ArcY");
		DebugArcY.Location = CaptureView->GetRelativeLocation();
		DebugArcY.Rotation = CaptureView->GetRelativeRotation() + FRotator{0.0f, 0.0f, 90.0f};
		DebugArcY.MinAngle = -TurningRange.Y;
		DebugArcY.MaxAngle = TurningRange.Y;
		DebugArcY.Color = FColor::Green;
		DebugArcY.Thickness = 0.5f;
		DebugArcY.Sections = 16.0f;
		DebugArcY.Radius = 24.0f;
	}
#endif
}

void ACCTVCamera::OnEnableStateChanged(const bool bIsEnabled)
{
	if (Monitor) Monitor->UpdateCameraStatic();
	SceneCapture->AddOrUpdateBlendable(TrackingPostProcess, 0.0f);
	bEnabled ? GetWorldTimerManager().UnPauseTimer(CaptureHandle) : GetWorldTimerManager().PauseTimer(CaptureHandle);
	bEnabled ? GetWorldTimerManager().UnPauseTimer(RefreshHandle) : GetWorldTimerManager().PauseTimer(RefreshHandle);

	for (const TObjectPtr<const AActor>& Actor : CullingActors)
	{
		if (USmartCullingComponent* Comp = Actor ? Actor->FindComponentByClass<USmartCullingComponent>() : nullptr)
		{
			bIsEnabled ? Comp->AddRenderRequest(this) : Comp->RemoveRenderRequest(this);
		}
	}
}

void ACCTVCamera::OnEnemyAudioPlayed(const AFRCharacter* Enemy, const UAudioComponent* Component)
{
	if (bEnabled && Monitor && Enemy && Component && Component->Sound && VisionCone->GetActorVisionState(Enemy, true) != EVisionConeState::None)
	{
		const float Volume = FMath::GetMappedRangeValueClamped(FVector2D(500.0f, VisionCone->MaxDistance),
			FVector2D(1.0f, 0.1f), FVector::Distance(VisionCone->GetComponentLocation(), Enemy->GetActorLocation()));

		Monitor->PlayMonitorAudio(Component->Sound, Volume);
	}
}
