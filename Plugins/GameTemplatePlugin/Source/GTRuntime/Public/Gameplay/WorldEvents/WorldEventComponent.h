// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "WorldEvents.h"
#include "InstancedStruct.h"
#include "Components/ActorComponent.h"
#include "WorldEventComponent.generated.h"

UCLASS(ClassGroup = (Game), meta = (BlueprintSpawnableComponent))
class GTRUNTIME_API UWorldEventComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UWorldEventComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (BaseStruct = "/Script/GTRuntime.WorldEventBase", ExcludeBaseStruct))
		TArray<FInstancedStruct> Events;

	UFUNCTION(BlueprintCallable, Category = "WorldEvents")
		void SetupEvents();

	UFUNCTION(BlueprintCallable, Category = "WorldEvents")
		void RunEvents();

protected:

	TSet<FWorldEventBase*> EventPtrs;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
