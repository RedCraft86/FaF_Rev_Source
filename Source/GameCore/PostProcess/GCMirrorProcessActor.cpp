// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCMirrorProcessActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PlanarReflection.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PostProcessComponent.h"
#include "Components/PlanarReflectionComponent.h"
#include "UserSettings/GCUserSettings.h"
#if WITH_EDITOR
#include "UObject/ConstructorHelpers.h"
#include "Components/BillboardComponent.h"
#endif

AGCMirrorProcessActor::AGCMirrorProcessActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

	SceneRoot = CreateDefaultSubobject<UBoxComponent>("SceneRoot");
	SceneRoot->SetCollisionProfileName(TEXT("Custom"));
	SceneRoot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SceneRoot->SetCollisionObjectType(ECC_WorldStatic);
	SceneRoot->SetCollisionResponseToAllChannels(ECR_Ignore);
	SceneRoot->SetVisibility(true);
	SetRootComponent(SceneRoot);
	
	PostProcess = CreateDefaultSubobject<UPostProcessComponent>("PostProcess");
	PostProcess->SetupAttachment(SceneRoot);
	
	PostProcess->Priority = 1.0f;
	PostProcess->bEnabled = true;
	PostProcess->bUnbound = false;
	PostProcess->BlendRadius = 0.0f;
	PostProcess->Settings.bOverride_ReflectionMethod = true;
	PostProcess->Settings.ReflectionMethod = EReflectionMethod::None;

#if WITH_EDITOR
	VisualIcon = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("VisualIcon");
	if (VisualIcon)
	{
		VisualIcon->SetRelativeScale3D_Direct(FVector(0.25f, 0.25f, 0.25f));
		VisualIcon->bIsScreenSizeScaled = true;
		VisualIcon->SetupAttachment(SceneRoot);
		const ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(
			TEXT("/Engine/EditorResources/S_ReflActorIcon.S_ReflActorIcon"));
		if (IconFinder.Succeeded())
		{
			VisualIcon->Sprite = IconFinder.Object;
		}
	}
#endif

	Priority = 1.0f;
	GameSettings = nullptr;
	bPointInBox = false;
}

void AGCMirrorProcessActor::BeginPlay()
{
	Super::BeginPlay();
	GameSettings = UGCUserSettings::Get();
	if (GameSettings)
	{
		ApplyState();
		GameSettings->OnDynamicApply.AddUObject(this, &AGCMirrorProcessActor::OnSettingChanged);
		GameSettings->OnManualApply.AddUObject(this, &AGCMirrorProcessActor::OnSettingChanged);
	}
}

void AGCMirrorProcessActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (const APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		const bool bInBox = UKismetMathLibrary::IsPointInBox(PCM->GetCameraLocation(),
			SceneRoot->Bounds.Origin, SceneRoot->Bounds.BoxExtent);

		if (bPointInBox != bInBox)
		{
			bPointInBox = bInBox;
			ApplyState();
		}
	}
}

void AGCMirrorProcessActor::ApplyState()
{
	if (!GameSettings) GameSettings = UGCUserSettings::Get();
	if (GameSettings)
	{
		PostProcess->Priority= Priority;
		PostProcess->bEnabled = GameSettings->GetMirrorQuality() > 0;

		const float ScreenPercent = GameSettings->GetMirrorQuality() * 25.0f;
		const bool bShouldRender = bPointInBox && PostProcess->bEnabled;
		for (const APlanarReflection* Reflection : PlanarReflections)
		{
			Reflection->GetPlanarReflectionComponent()->ScreenPercentage = ScreenPercent;
			Reflection->GetPlanarReflectionComponent()->bCaptureEveryFrame = bShouldRender;
			Reflection->GetPlanarReflectionComponent()->bCaptureOnMovement = bShouldRender;
			Reflection->GetPlanarReflectionComponent()->bAlwaysPersistRenderingState = !bShouldRender;
			Reflection->GetPlanarReflectionComponent()->SetVisibility(bShouldRender);
		}
	}
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void AGCMirrorProcessActor::OnSettingChanged(UGCUserSettings* Settings)
{
	GameSettings = Settings;
	ApplyState();
}
