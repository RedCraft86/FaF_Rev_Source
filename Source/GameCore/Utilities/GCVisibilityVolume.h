// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

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
	
	void ApplyState(const bool bVisible);
	virtual void Tick(float DeltaSeconds) override;
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override
	{
		BrushColor = Color.ToFColor(true);
	}
#endif
};
