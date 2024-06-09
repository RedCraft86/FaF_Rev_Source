// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "WorldEventComponent.h"
#include "WorldEventActor.generated.h"

UCLASS()
class GTRUNTIME_API AWorldEventActor final : public AActor
{
	GENERATED_BODY()

public:

	AWorldEventActor();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UWorldEventComponent> WorldEventComponent;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "WorldEvents")
		void RunEvents() const;
};
