// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "RCPrimitiveTypes.h"
#include "GameFramework/Actor.h"
#include "GCTriggerField.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBPOnTriggered);

UCLASS(DisplayName = "Trigger Field")
class GAMECORE_API AGCTriggerField : public AActor
{
	GENERATED_BODY()

public:

	AGCTriggerField();

	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bStartEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bSingleUse;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (MakeEditWidget = true))
		FVector BoxExtent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		FRCPrimitiveCollision Collision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (BaseStruct = "/Script/GameCore.GCEvent", ExcludeBaseStruct))
		TArray<FInstancedStruct> Events;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Triggered"))
		FBPOnTriggered OnTriggeredBP;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction Field")
		void SetEnabled(const bool bEnabled);

	DECLARE_MULTICAST_DELEGATE(FNativeOnTriggered)
	FNativeOnTriggered OnTriggered;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
protected:
	
	bool bUsed;
};
