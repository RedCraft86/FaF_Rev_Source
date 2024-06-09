// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "SmartCulling.h"
#include "GameFramework/Volume.h"
#include "SmartCullingVolume.generated.h"

UCLASS(meta = (AllowedCategories = "Tools"))
class GTRUNTIME_API ASmartCullingVolume final : public AVolume
{
	GENERATED_BODY()

public:

	ASmartCullingVolume();

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bInvert;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSet<TSoftObjectPtr<AActor>> TargetActors;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Tools", meta = (HideAlphaChannel))
		FLinearColor Color = FLinearColor::MakeRandomColor();
	
	UPROPERTY(EditAnywhere, Category = "Tools|Finder")
		bool bWithinBounds = true;

	UPROPERTY(EditAnywhere, Category = "Tools|Finder")
		FName FindTag = NAME_None;
#endif
private:
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Tools")
		void RandomizeColor();
	
	UFUNCTION(CallInEditor, Category = "Tools")
		void FindActors();
#endif

	uint8 Ticks = 0;
	UPROPERTY(Transient) TObjectPtr<APlayerCameraManager> CamManager;
	UPROPERTY(Transient) TSet<TObjectPtr<USmartCullingComponent>> CullingComponents;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override
	{
		Super::OnConstruction(Transform);
		BrushColor = Color.ToFColor(true);
	}
#endif
};
