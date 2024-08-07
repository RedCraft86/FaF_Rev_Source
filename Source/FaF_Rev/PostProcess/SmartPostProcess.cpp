// Copyright (C) RedCraft86. All Rights Reserved.

#include "SmartPostProcess.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameSettings/GameSettings.h"
#if WITH_EDITOR
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/BillboardComponent.h"
#endif

void FFRBloomChoice::ApplyChoice(FPostProcessSettings& Settings, const bool bFancy) const
{
#if WITH_EDITOR
	if (!FApp::IsGame())
	{
		bStartFancy ? FancyBloom.AlterPostProcess(Settings) : SimpleBloom.AlterPostProcess(Settings);
	}
	else
#endif
	{
		bFancy ? FancyBloom.AlterPostProcess(Settings) : SimpleBloom.AlterPostProcess(Settings);
	}
}

ASmartPostProcess::ASmartPostProcess()
	: UpdateInterval(0.05f), Priority(0.0f), BlendRadius(100.0f), BlendWeight(1.0f), bEnabled(true), bUnbound(true)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.TickInterval = 0.05f;
	bAllowTickBeforeBeginPlay = true;

	SceneRoot = CreateDefaultSubobject<UBoxComponent>("SceneRoot");
	SceneRoot->SetCollisionProfileName(TEXT("NoCollision"));
	SceneRoot->SetVisibility(false);
	SetRootComponent(SceneRoot);

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>("PostProcess");
	PostProcess->SetupAttachment(SceneRoot);
	PostProcess->bUnbound = true;

#if WITH_EDITOR
	VisualIcon = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("VisualIcon");
	if (VisualIcon)
	{
		VisualIcon->SetRelativeScale3D_Direct(FVector(0.25f, 0.25f, 0.25f));
		VisualIcon->bIsScreenSizeScaled = true;
		VisualIcon->SetupAttachment(SceneRoot);
		const ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(
			TEXT("/Engine/EditorResources/S_BoxReflectionCapture.S_BoxReflectionCapture"));
		if (IconFinder.Succeeded())
		{
			VisualIcon->Sprite = IconFinder.Object;
		}
	}
#endif

	Settings.bOverride_AutoExposureMethod = true;
	Settings.AutoExposureMethod = AEM_Manual;

	Settings.bOverride_AutoExposureBias = true;
	Settings.AutoExposureBias = 10.0f;
}

#if WITH_EDITOR
void ASmartPostProcess::CopyFromTarget()
{
#if WITH_EDITORONLY_DATA
	if (CopyTarget.LoadSynchronous())
	{
		FPostProcessSettings NewSettings = Settings;
		if (const ASmartPostProcess* PPActor = Cast<ASmartPostProcess>(CopyTarget.LoadSynchronous()))
		{
			NewSettings = PPActor->Settings;
			Priority = PPActor->Priority;
			BlendRadius = PPActor->BlendRadius;
			BlendWeight = PPActor->BlendWeight;
			bUnbound = PPActor->bUnbound;
		}
		else if (const APostProcessVolume* PPVolume = Cast<APostProcessVolume>(CopyTarget.LoadSynchronous()))
		{
			NewSettings = PPVolume->Settings;
			Priority = PPVolume->Priority;
			BlendRadius = PPVolume->BlendRadius;
			BlendWeight = PPVolume->BlendWeight;
			bUnbound = PPVolume->bUnbound;
		}
		else if (const ACameraActor* CamActor = Cast<ACameraActor>(CopyTarget.LoadSynchronous()))
		{
			NewSettings = CamActor->GetCameraComponent()->PostProcessSettings;
			BlendWeight = CamActor->GetCameraComponent()->PostProcessBlendWeight;
		}
		else if (const UPostProcessComponent* PPComp = CopyTarget ? CopyTarget->FindComponentByClass<UPostProcessComponent>() : nullptr)
		{
			NewSettings = PPComp->Settings;
			Priority = PPComp->Priority;
			BlendRadius = PPComp->BlendRadius;
			BlendWeight = PPComp->BlendWeight;
			bUnbound = PPComp->bUnbound;
		}
		else if (const UCameraComponent* CamComp = CopyTarget ? CopyTarget->FindComponentByClass<UCameraComponent>() : nullptr)
		{
			NewSettings = CamComp->PostProcessSettings;
			BlendWeight = CamComp->PostProcessBlendWeight;
		}

		if (bPreserveExposure)
		{
			NewSettings.bOverride_AutoExposureMethod = Settings.bOverride_AutoExposureMethod;
			NewSettings.AutoExposureMethod = Settings.AutoExposureMethod;

			NewSettings.bOverride_AutoExposureBias = Settings.bOverride_AutoExposureBias;
			NewSettings.AutoExposureBias = Settings.AutoExposureBias;
		}

		CopyTarget = nullptr;
		Settings = NewSettings;
		SceneRoot->SetVisibility(!bUnbound);
		ApplySettings();
	}
#endif
}
#endif

USmartBlendable* ASmartPostProcess::FindSmartBlendable(const FName InName) const
{
	return Blendables.Contains(InName) ? Blendables.FindRef(InName) : nullptr;
}

USmartBlendable* ASmartPostProcess::AddSmartBlendable(const FName InName, const TSubclassOf<USmartBlendable> InClass)
{
	if (USmartBlendable* Blendable = FindSmartBlendable(InName))
	{
		Blendable->bEnabled = true;
		return Blendable;
	}

	FName Name = InName;
	if (InName.IsNone()) Name = *FGuid::NewGuid().ToString();
	if (USmartBlendable* NewBlendable = NewObject<USmartBlendable>(this, InClass))
	{
		NewBlendable->PostProcessActor = this;
		NewBlendable->OnInitialize();
		
		Blendables.Add(Name, NewBlendable);
		return NewBlendable;
	}

	return nullptr;
}

void ASmartPostProcess::ApplySettings()
{
	Bloom.ApplyChoice(Settings, !GameSettings || GameSettings->GetUseFancyBloom());
	
	PostProcess->Settings = Settings;
	for (const TPair<FName, USmartBlendable*>& Pair : Blendables)
	{
		if (Pair.Value && Pair.Value->bEnabled)
			PostProcess->Settings.AddBlendable(Pair.Value->GetDynamicMaterialInstance(), 1.0f);
    }
    
	PostProcess->Priority = Priority;
	PostProcess->BlendWeight = BlendWeight;
	PostProcess->BlendRadius = BlendRadius;
	PostProcess->bEnabled = bEnabled;
	PostProcess->bUnbound = bUnbound;
}

void ASmartPostProcess::UpdateBlendables()
{
	for (const TPair<FName, USmartBlendable*>& Pair : Blendables)
	{
		if (Pair.Value)
		{
			Pair.Value->PostProcessActor = this;
			Pair.Value->OnInitialize();
		}
	}
}

void ASmartPostProcess::BeginPlay()
{
	Super::BeginPlay();
	GameSettings = UGameSettings::Get();
	SetActorTickInterval(UpdateInterval);
	UpdateBlendables();
}

void ASmartPostProcess::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (const TPair<FName, USmartBlendable*>& Pair : Blendables)
	{
		if (Pair.Value) Pair.Value->OnTick(DeltaTime);
	}
	ApplySettings();
}

void ASmartPostProcess::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SceneRoot->SetVisibility(!bUnbound);
	UpdateBlendables();
	ApplySettings();
}

#if WITH_EDITOR
void ASmartPostProcess::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
#if WITH_EDITORONLY_DATA
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, CopyTarget))
	{
		if (CopyTarget == this) CopyTarget = nullptr;
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, Blendables))
	{
		for (TPair<FName, USmartBlendable*>& Pair : Blendables)
		{
			if (Pair.Key.IsNone()) Pair.Key = *FGuid::NewGuid().ToString();
			if (IsValid(Pair.Value)) Pair.Value->PostProcessActor = this;
		}
	}
#endif
}
#endif
