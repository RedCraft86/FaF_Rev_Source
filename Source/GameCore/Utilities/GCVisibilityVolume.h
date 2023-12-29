// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Components/SceneCaptureComponent.h"
#include "GameFramework/Volume.h"
#include "GCVisibilityVolume.generated.h"

UCLASS(DisplayName = "Visibility Volume", HideCategories = (Collision, PathTracing, BrushSettings, Physics, Input, HLOD, PathTracing, Navigation, Tags, Cooking, Replication, Networking, Mobile, RayTracing, AssetUserData, LevelInstance))
class GAMECORE_API AGCVisibilityVolume final : public AVolume
{
	GENERATED_BODY()

public:

	AGCVisibilityVolume();

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bInvert;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSet<AActor*> Targets;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSet<AActor*> ConditionActors;
	
	// UPROPERTY(EditAnywhere, Category = "Settings")
	// 	TSet<AActor*> SceneCaptureActors;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (HideAlphaChannel))
		FLinearColor Color = FLinearColor::MakeRandomColor();
	
	UPROPERTY(EditAnywhere, Category = "TargetFinder")
		bool bWithinBounds = true;

	UPROPERTY(EditAnywhere, Category = "TargetFinder")
		FName FindTag = NAME_None;
#endif
#if WITH_EDITOR
protected:
	UFUNCTION(CallInEditor, Category = "Settings")
		void RandomizeColor();
	
	UFUNCTION(CallInEditor, Category = "TargetFinder")
		void FindActors();
#endif
private:
	
	bool bCachedState;
	TMap<TWeakObjectPtr<AActor>, bool> CachedTickStates;
	// TSet<TWeakObjectPtr<USceneCaptureComponent>> Captures;
	
	void ApplyState(const bool bVisible);
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnConstruction(const FTransform& Transform) override
	{
		Super::OnConstruction(Transform);
#if WITH_EDITOR
		BrushColor = Color.ToFColor(true);
#endif
		// Captures.Empty();
		// for (const AActor* Actor : SceneCaptureActors)
		// {
		// 	if (!Actor) continue;
		//
		// 	TArray<UActorComponent*> Components;
		// 	Actor->GetComponents(USceneCaptureComponent::StaticClass(), Components);
		// 	for (UActorComponent* Component : Components)
		// 	{
		// 		if (USceneCaptureComponent* Capture = Cast<USceneCaptureComponent>(Component))
		// 		{
		// 			Captures.Add(Capture);
		// 		}
		// 	}
		// }
	}

};
