// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "RCPrimitiveTypes.h"
#include "GameFramework/Actor.h"
#include "Interfaces/GCInteractionInterface.h"
#include "GCInteractField.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBPOnInteracted);

UCLASS(DisplayName = "Interaction Field")
class GAMECORE_API AGCInteractField : public AActor, public IGCInteractionInterface
{
	GENERATED_BODY()

public:

	AGCInteractField();

	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bStartEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bSingleUse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		FText Label;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (MakeEditWidget = true))
		FVector BoxExtent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		FRCPrimitiveCollision Collision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (BaseStruct = "/Script/GameCore.GCEvent", ExcludeBaseStruct))
		TArray<FInstancedStruct> Events;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Interacted"))
		FBPOnInteracted OnInteractedBP;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction Field")
		void SetEnabled(const bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "Interaction Field")
		bool GetEnabled() const { return GetActorEnableCollision(); }

	UFUNCTION(BlueprintPure, Category = "Interaction Field")
		bool HasEverBeenUsed() const { return UseCount > 0; }

	UFUNCTION(BlueprintPure, Category = "Interaction Field")
		int32 GetUsedCount() const { return UseCount; }
	
	DECLARE_MULTICAST_DELEGATE(FNativeOnInteracted)
	FNativeOnInteracted OnInteracted;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult) override;
	virtual bool GetInteractionInfo_Implementation(FText& DisplayName) override;
	
protected:

	int32 UseCount;
	bool bUsed;
};
