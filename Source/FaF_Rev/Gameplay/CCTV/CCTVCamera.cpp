// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVCamera.h"
#include "CCTVMonitor.h"
#include "CharacterAI/FREnemyBase.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#if WITH_EDITOR
#include "EngineUtils.h"
#include "Components/DebugShapesComponent.h"
#endif

ACCTVCamera::ACCTVCamera() : TurningRate(FVector2D::UnitVector), TurningRange(25.0f, 10.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	CaptureView = CreateDefaultSubobject<USceneComponent>("CaptureView");
	CaptureView->SetupAttachment(SceneRoot);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
	SceneCapture->SetupAttachment(CaptureView);
	SceneCapture->bAlwaysPersistRenderingState = true;
	SceneCapture->bCaptureEveryFrame = false;
	
	TrackingVolume = CreateDefaultSubobject<UBoxComponent>("TrackingVolume");
	TrackingVolume->SetupAttachment(SceneRoot);
	TrackingVolume->ShapeColor = FColor::MakeRandomColor();

	bEnabled = false;
	bStartWithCollisionEnabled = false;
	TrackingArea.SetTranslation(TrackingArea.GetTranslation() + FVector{50.0f, 0.0f, 0.0f});

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
		bSeeEnemies = TrackingVolume->IsOverlappingActor(Enemy);
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
	
	GetWorldTimerManager().SetTimer(CaptureHandle, this, &ACCTVCamera::CaptureScene, 0.05f, true);
	GetWorldTimerManager().PauseTimer(CaptureHandle);
	
	GetWorldTimerManager().SetTimer(RefreshHandle, this, &ACCTVCamera::RefreshEnemyState, 0.5f, true);
	GetWorldTimerManager().PauseTimer(RefreshHandle);
	
	Enemies.Remove(nullptr);
	for (const TObjectPtr<const AFREnemyBase>& Enemy : Enemies)
	{
		if (!Enemy) continue;
	}
}

void ACCTVCamera::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	TrackingVolume->SetRelativeTransform(TrackingArea);
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

		FDebugBoxData& DebugTrackingVolume = DebugShapes->DebugBoxes.FindOrAdd("Tracking");
		DebugTrackingVolume.Location = TrackingVolume->GetRelativeLocation();
		DebugTrackingVolume.Rotation = TrackingVolume->GetRelativeRotation();
		DebugTrackingVolume.Extents = TrackingVolume->GetScaledBoxExtent();
		DebugTrackingVolume.Color = TrackingVolume->ShapeColor;
	}
#endif
}

void ACCTVCamera::OnEnableStateChanged(const bool bIsEnabled)
{
	if (Monitor) Monitor->UpdateCameraStatic();
	bEnabled ? GetWorldTimerManager().UnPauseTimer(CaptureHandle) : GetWorldTimerManager().PauseTimer(CaptureHandle);
	bEnabled ? GetWorldTimerManager().UnPauseTimer(RefreshHandle) : GetWorldTimerManager().PauseTimer(RefreshHandle);
}
