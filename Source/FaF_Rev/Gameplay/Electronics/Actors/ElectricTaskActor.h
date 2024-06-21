// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "WorldEvents/WorldEvents.h"
#include "Electronics/ElectricActor.h"
#include "ElectricTaskActor.generated.h"

UCLASS()
class FAF_REV_API AElectricTaskActor final : public AElectricActorBase
{
	GENERATED_BODY()

public:

	AElectricTaskActor() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (BaseStruct = "/Script/GTRuntime.WorldEventBase", ExcludeBaseStruct, ShowTreeView))
		TArray<FInstancedStruct> ActivateEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (BaseStruct = "/Script/GTRuntime.WorldEventBase", ExcludeBaseStruct, ShowTreeView))
		TArray<FInstancedStruct> DeactivateEvents;

	UFUNCTION(BlueprintCallable, Category = "ElectricActor|Task")
		void SetupEvents();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnStateChanged(const bool bState) override;
#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:

	TSet<FWorldEventBase*> ActivateEventPtrs;
	TSet<FWorldEventBase*> DeactivateEventPtrs;
};
