// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "DebugNavPath.generated.h"

UCLASS(NotBlueprintable, meta = (HideCategories = "Collision,Actor"))
class GTRUNTIME_API ADebugNavPath final : public AActor
{
	GENERATED_BODY()

public:

	ADebugNavPath();

	UPROPERTY(VisibleDefaultsOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, Category = "Settings", DuplicateTransient, meta = (HideAlphaChannel = true))
		FColor Color = FColor::MakeRandomColor();

	UPROPERTY(EditAnywhere, Category = "Settings", DuplicateTransient)
		TSoftObjectPtr<ADebugNavPath> NextPath;
	
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
#endif
};
