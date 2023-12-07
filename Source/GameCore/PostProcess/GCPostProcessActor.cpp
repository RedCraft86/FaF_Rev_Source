// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCPostProcessActor.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/PlayerController.h"
#include "GCPostProcessBlendable.h"
#include "RCCVarLibrary.h"
#include "UserSettings/GCUserSettings.h"
#if WITH_EDITOR
#include "Editor.h"
#include "EditorViewportClient.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BillboardComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"
#endif

const FPostProcessSettings& FGCPostProcessSettings::GetScaledSettings(const bool bFancyBloom)
{
	if (bFancyBloom)
	{
		FancyBloom.AlterPostProcess(Settings);
	}
	else
	{
		SimpleBloom.AlterPostProcess(Settings);
	}

	return Settings;
}

AGCPostProcessActor::AGCPostProcessActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bAllowTickBeforeBeginPlay = true;

	SceneRoot = CreateDefaultSubobject<UBoxComponent>("SceneRoot");
	SceneRoot->SetCollisionProfileName(TEXT("Custom"));
	SceneRoot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SceneRoot->SetCollisionObjectType(ECC_WorldStatic);
	SceneRoot->SetCollisionResponseToAllChannels(ECR_Ignore);
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

	BlendRadius = 100.0f;
	BlendWeight = 1.0f;
	bEnabled = true;
	bUnbound = true;
	Blendables = {};
	Settings = {};
}

#if WITH_EDITOR
void AGCPostProcessActor::CopyFromTarget()
{
#if WITH_EDITORONLY_DATA
	if (CopyTarget)
	{
		FGCPostProcessSettings NewSettings;
		NewSettings.Settings = *Settings;
		NewSettings.BloomShape = Settings.BloomShape;
	
		if (const AGCPostProcessActor* PPActor = Cast<AGCPostProcessActor>(CopyTarget))
		{
			NewSettings = PPActor->Settings;
			Priority = PPActor->Priority;
			BlendRadius = PPActor->BlendRadius;
			BlendWeight = PPActor->BlendWeight;
			bUnbound = PPActor->bUnbound;
		}
		else if (const APostProcessVolume* PPVolume = Cast<APostProcessVolume>(CopyTarget))
		{
			NewSettings.Settings = PPVolume->Settings;
			Priority = PPVolume->Priority;
			BlendRadius = PPVolume->BlendRadius;
			BlendWeight = PPVolume->BlendWeight;
			bUnbound = PPVolume->bUnbound;
		}
		else if (const ACameraActor* CamActor = Cast<ACameraActor>(CopyTarget))
		{
			NewSettings.Settings = CamActor->GetCameraComponent()->PostProcessSettings;
			BlendWeight = CamActor->GetCameraComponent()->PostProcessBlendWeight;
		}
		else if (const UPostProcessComponent* PPComp = CopyTarget ? CopyTarget->FindComponentByClass<UPostProcessComponent>() : nullptr)
		{
			NewSettings.Settings = PPComp->Settings;
			Priority = PPComp->Priority;
			BlendRadius = PPComp->BlendRadius;
			BlendWeight = PPComp->BlendWeight;
			bUnbound = PPComp->bUnbound;
		}
		else if (const UCameraComponent* CamComp = CopyTarget ? CopyTarget->FindComponentByClass<UCameraComponent>() : nullptr)
		{
			NewSettings.Settings = CamComp->PostProcessSettings;
			BlendWeight = CamComp->PostProcessBlendWeight;
		}

		if (bPreserveExposure)
		{
			NewSettings.Settings.bOverride_AutoExposureMethod = (*Settings).bOverride_AutoExposureMethod;
			NewSettings.Settings.AutoExposureMethod = (*Settings).AutoExposureMethod;

			NewSettings.Settings.bOverride_AutoExposureBias = (*Settings).bOverride_AutoExposureBias;
			NewSettings.Settings.AutoExposureBias = (*Settings).AutoExposureBias;
		}

		CopyTarget = nullptr;
		Settings = NewSettings;
		ApplySettings();
	}
#endif
}
#endif

UGCPostProcessBlendable* AGCPostProcessActor::FindSmartBlendable(const TSubclassOf<UGCPostProcessBlendable> InClass) const
{
	for (UGCPostProcessBlendable* Material : Blendables)
	{
		if (Material && Material->IsA(InClass))
		{
			return Material;
		}
	}

	return nullptr;
}

UGCPostProcessBlendable* AGCPostProcessActor::AddSmartBlendable(const TSubclassOf<UGCPostProcessBlendable> InClass)
{
	if (UGCPostProcessBlendable* Blendable = FindSmartBlendable(InClass))
	{
		Blendable->bEnabled = true;
		return Blendable;
	}

	if (UGCPostProcessBlendable* NewBlendable = NewObject<UGCPostProcessBlendable>(this, InClass))
	{
		NewBlendable->PostProcessActor = this;
		NewBlendable->OnInit();
		
		Blendables.Add(NewBlendable);
		return NewBlendable;
	}

	return nullptr;
}

void AGCPostProcessActor::BeginPlay()
{
	Super::BeginPlay();

	Blendables.Remove(nullptr);
	ApplyBlendables();
}

void AGCPostProcessActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ApplySettings();

	for (UGCPostProcessBlendable* Material : Blendables)
	{
		if (Material)
		{
			Material->OnTick(DeltaTime);
		}
	}
}

void AGCPostProcessActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SceneRoot->SetVisibility(!bUnbound);
	ApplySettings();

	ApplyBlendables();
#if WITH_EDITORONLY_DATA
	if (CopyTarget == this)
	{
		CopyTarget = nullptr;
	}
#endif
}

#if WITH_EDITOR
bool AGCPostProcessActor::CanEditChange(const FProperty* InProperty) const
{
	const bool bSuper = Super::CanEditChange(InProperty);
	if (!InProperty)
	{
		return bSuper;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FGCPostProcessSettings, BloomShape))
	{
		return bSuper && PostProcess->Settings.BloomMethod == BM_SOG;
	}

	return bSuper;
}

void AGCPostProcessActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AGCPostProcessActor, Blendables))
	{
		if (const int32 LastIdx = Blendables.Num() - 1; LastIdx >= 0)
		{
			if (const UGCPostProcessBlendable* LastItem = Blendables[LastIdx])
			{
				for (const UGCPostProcessBlendable* Blendable : Blendables)
				{
					if (Blendable && Blendable != LastItem && Blendable->GetClass() == LastItem->GetClass())
					{
						Blendables.RemoveAt(LastIdx);
						break;
					}
				}
			}
		}
	}
}
#endif

void AGCPostProcessActor::ApplySettings()
{
#if WITH_EDITOR
	if (FApp::IsGame())
#endif
	{
		const UGCUserSettings* GameSettings = UGCUserSettings::Get();
		PostProcess->Settings = Settings.GetScaledSettings(GameSettings ? GameSettings->GetUseFancyBloom() : true);
	}
#if WITH_EDITOR
	else
	{
		PostProcess->Settings = Settings.GetScaledSettings(Settings.bStartFancy);
	}
#endif
	PostProcess->Priority = Priority;
	PostProcess->BlendWeight = BlendWeight;
	PostProcess->BlendRadius = BlendRadius;
	PostProcess->bEnabled = bEnabled;
	PostProcess->bUnbound = bUnbound;

	const float CVarBlend = CalculateCVarBlend();
	if (bUnbound || DistToPoint < BlendRadius + 10.0f)
	{
		if (IConsoleVariable* BloomCross = URCCVarLibrary::FindCVar(TEXT("r.GaussianBloom.Cross")))
		{
			BloomCross->Set(FMath::Lerp(0.0f, Settings.BloomShape, CVarBlend), ECVF_SetByConsole);
		}
	}
}

void AGCPostProcessActor::ApplyBlendables()
{
	for (UGCPostProcessBlendable* Material : Blendables)
	{
		if (Material)
		{
			Material->PostProcessActor = this;
			Material->OnInit();
		}
	}
}

float AGCPostProcessActor::CalculateCVarBlend()
{
	if (PostProcess->bUnbound)
	{
		return FMath::Clamp(PostProcess->BlendWeight, 0.0f, 1.0f);
	}

	FVector CameraLocation = FVector::ZeroVector;
	if (!GetCameraPosition(CameraLocation))
	{
		return 0.0f;
	}
	
	DistToPoint = 0.0f;
	PostProcess->EncompassesPoint(CameraLocation, 0.0f, &DistToPoint);
	
	if (DistToPoint >= 0.0f)
	{
		if (DistToPoint > PostProcess->BlendRadius)
		{
			return 0.0f;
		}
		
		if (PostProcess->BlendRadius >= 1.0f)
		{
			float LocalWeight = FMath::Clamp(PostProcess->BlendWeight, 0.0f, 1.0f);
			LocalWeight *= 1.0f - DistToPoint / PostProcess->BlendRadius;

			if(LocalWeight >= 0 && LocalWeight <= 1.0f)
			{
				return LocalWeight;
			}
		}
	}

	return 0.0f;
}

bool AGCPostProcessActor::GetCameraPosition(FVector& OutCameraLocation) const
{
#if WITH_EDITOR
	if (!FApp::IsGame() && GEditor)
	{
		const FViewport* Viewport = GEditor->GetActiveViewport();
		const FViewportClient* VClient = Viewport ? Viewport->GetClient() : nullptr;
		if (const FEditorViewportClient* Client = VClient ? static_cast<const FEditorViewportClient*>(VClient) : nullptr)
		{
			OutCameraLocation = Client->GetViewLocation();
			return true;
		}
	}
	else
#endif
	{
		if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			FRotator OutRotation = FRotator::ZeroRotator;
			PC->GetPlayerViewPoint(OutCameraLocation, OutRotation);
			return true;
		}
	}

	OutCameraLocation = FVector::ZeroVector;
	return false;
}
