// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GCInventoryTypes.h"
#include "RCPrimitiveTypes.h"
#include "GameFramework/Actor.h"
#include "Interfaces/GCInteractionInterface.h"
#include "GCGrabbableItem.generated.h"

UCLASS(DisplayName = "Grabbable Item")
class GAMECORE_API AGCGrabbableItem : public AActor, public IGCInteractionInterface
{
	GENERATED_BODY()

public:

	AGCGrabbableItem();

	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UInstancedStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bEnabled;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		FGCInvItemID ItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
		FText ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		uint8 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		bool bAutoEquip;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (MakeEditWidget = true))
		TArray<FTransform> Meshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		FRCPrimitiveCollision Collision;

	UFUNCTION(BlueprintCallable, Category = "GrabbableItem")
		void SetEnabled(const bool bNewEnabled);
	
protected:
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		SetEnabled(bEnabled);
	}
	
	virtual void UpdateActor(const uint8 ItemsLeft);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual bool GetInteractionInfo_Implementation(FText& DisplayName) override;
	virtual void OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult) override;
};
