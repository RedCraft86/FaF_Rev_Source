// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GTActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorEnableChangedSignature, const bool, bEnabled);

#define ON_ENABLE_STATE_CHANGED { OnEnableStateChanged(bEnabled); OnEnableStateChangedEvent.Broadcast(bEnabled); OnEnableStateChangedBP.Broadcast(bEnabled); }

UCLASS(Abstract)
class GTRUNTIME_API AGTActor : public AActor
{
	GENERATED_BODY()

public:

	AGTActor();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Enable State Changed")
		FActorEnableChangedSignature OnEnableStateChangedBP;
	
	UFUNCTION(BlueprintCallable, Category = "Actor")
		virtual void SetEnabled(const bool bInEnabled);
	
	UFUNCTION(BlueprintPure, Category = "Actor")
		bool IsEnabled() const { return bEnabled; }
	
	UFUNCTION(BlueprintPure, Category = "Actor")
		FGuid GetRuntimeGuid() const { return RuntimeGuid; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FActorEnableChanged, const bool);
	FActorEnableChanged OnEnableStateChangedEvent;

protected:

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -10))
		bool bEnabled;

	UPROPERTY(EditAnywhere, Category = "Collision")
		bool bStartWithCollisionEnabled;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor", NonPIEDuplicateTransient, TextExportTransient, NonTransactional)
		FGuid RuntimeGuid;
	
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnEnableStateChanged(const bool bIsEnabled);
};
