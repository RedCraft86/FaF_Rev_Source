// Copyright (C) RedCraft86. All Rights Reserved.

#include "Optimization/SmartCullingVolume.h"
#include "Kismet/GameplayStatics.h"
#include "GTRuntime.h"
#if WITH_EDITORONLY_DATA
#include "EngineUtils.h"
#endif

ASmartCullingVolume::ASmartCullingVolume() : bInvert(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

#if WITH_EDITORONLY_DATA
	bRunConstructionScriptOnDrag = true;
#endif
#if WITH_EDITOR
	RandomizeColor();
#endif
}

#if WITH_EDITOR
void ASmartCullingVolume::RandomizeColor()
{
#if WITH_EDITORONLY_DATA
	bColored = true;
	Color = FLinearColor::MakeRandomColor();
	BrushColor = Color.ToFColor(true);
	MarkComponentsRenderStateDirty();
#endif
}

void ASmartCullingVolume::FindActors()
{
#if WITH_EDITORONLY_DATA
	for (const AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!USmartCullingComponent::GetSmartCulling(Actor)) continue;
		if (!bWithinBounds || (bWithinBounds && EncompassesPoint(Actor->GetActorLocation())))
		{
			if (FindTag.IsNone() || Actor->ActorHasTag(FindTag))
			{
				TargetActors.Add(Actor);
			}
		}
	}
	
	TargetActors.Remove(nullptr);
#endif
}
#endif

void ASmartCullingVolume::BeginPlay()
{
	Super::BeginPlay();
	CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	for (const TSoftObjectPtr<AActor> Actor : TargetActors)
	{
		if (!Actor.LoadSynchronous()) continue;
		const AActor* ActorPtr = Actor.LoadSynchronous();
		if (USmartCullingComponent* Comp = USmartCullingComponent::GetSmartCulling(ActorPtr))
		{
			if (!Comp->IsDisabled()) CullingComponents.Add(Comp);
		}
#if WITH_EDITOR
		else
		{
			UE_LOG(GTRuntime, Error, TEXT("No Smart Culling Component found on Actor %s (%s) [%s]"),
				*ActorPtr->GetName(), *ActorPtr->GetActorLabel(), *ActorPtr->GetClass()->GetName());
		}
#endif
	}
}

void ASmartCullingVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!CamManager) return;

	if (Ticks++ > 10) // Every 10 ticks, look for the camera manager again just in case it changed
	{
		CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	}

	bool bShouldRender = EncompassesPoint(CamManager->GetCameraLocation());
	if (bInvert) bShouldRender = !bShouldRender;
	
	for (const TObjectPtr<USmartCullingComponent> Component : CullingComponents)
	{
		if (!Component || Component->IsDisabled()) continue;
		bShouldRender ? Component->AddRenderRequest(this) : Component->RemoveRenderRequest(this);
	}
}
