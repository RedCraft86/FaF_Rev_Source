// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FRPlayer.h"
#include "GameFramework/Actor.h"
#include "EquipmentActor.generated.h"

UCLASS(Abstract)
class FAF_REV_API AEquipmentActor : public AActor
{
	GENERATED_BODY()

public:

	AEquipmentActor() : bEquipped(false)
	{
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.bStartWithTickEnabled = false;

		SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
		SetRootComponent(SceneRoot);
	}

	UPROPERTY(VisibleAnywhere, Category = "Subobject")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "EquipmentActor")
		TObjectPtr<class UInventoryComponent> Inventory;

	UPROPERTY(BlueprintReadOnly, Category = "EquipmentActor")
		TObjectPtr<AFRPlayerBase> PlayerChar;
	
	UPROPERTY(VisibleAnywhere, Category = "EquipmentActor")
		FGuid ItemID;

	UPROPERTY(VisibleAnywhere, Category = "EquipmentActor")
		bool bEquipped;
	
	UFUNCTION(BlueprintImplementableEvent)
		void OnUse();

	UFUNCTION(BlueprintImplementableEvent)
		void OnStartAltUse();

	UFUNCTION(BlueprintImplementableEvent)
		void OnEndAltUse();

	UFUNCTION(BlueprintImplementableEvent)
		void EventEquip();
	
	UFUNCTION(BlueprintNativeEvent)
		void OnEquip();
	void OnEquip_Implementation()
	{
		AttachToComponent(PlayerChar->EquipmentRoot, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
	
	UFUNCTION(BlueprintNativeEvent)
		void OnUnequip();
	void OnUnequip_Implementation()
	{
		SetActorLocation(FVector::ZeroVector);
		GetWorldTimerManager().SetTimerForNextTick([WEAK_THIS]()
		{
			if (WeakThis.IsValid()) WeakThis->K2_DestroyActor();
		});
	}
};
