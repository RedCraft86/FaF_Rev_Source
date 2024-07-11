// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/DebugShapesComponent.h"
#include "DebugNavPath.generated.h"

UCLASS(NotBlueprintable, meta = (HideCategories = "Collision,Actor"))
class GTRUNTIME_API ADebugNavPath final : public AActor
{
	GENERATED_BODY()

public:

	ADebugNavPath();

	UPROPERTY(VisibleDefaultsOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Subobjects")
		TObjectPtr<UDebugShapesComponent> ShapeComponent;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (HideAlphaChannel = true))
		FLinearColor Color = FLinearColor::MakeRandomColor();

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (HideAlphaChannel = true))
		TSoftObjectPtr<ADebugNavPath> NextPath;
	
protected:

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
