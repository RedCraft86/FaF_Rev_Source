// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCGrabbableItem.h"
#include "RCRuntimeLibrary.h"
#include "GCInventoryManager.h"
#include "Components/InstancedStaticMeshComponent.h"

AGCGrabbableItem::AGCGrabbableItem()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	MeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(GetRootComponent());

#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif

	bEnabled = true;
	ItemID = {};
	ItemName = FText::GetEmpty();
	Amount = 1;
	bAutoEquip = true;
	Meshes = {};
	Collision.SetCollisionProfile("BlockAllDynamic");
}

void AGCGrabbableItem::SetEnabled(const bool bNewEnabled)
{
	bEnabled = bNewEnabled;
	SetActorHiddenInGame(!bNewEnabled);
	SetActorEnableCollision(bNewEnabled);
	SetActorTickEnabled(PrimaryActorTick.bStartWithTickEnabled && bNewEnabled);
}

#if WITH_EDITOR
bool AGCGrabbableItem::CanEditChange(const FProperty* InProperty) const
{
	const bool bSuper = Super::CanEditChange(InProperty);
	if (InProperty)
	{
		const bool bIsEquipment = UGCInventoryManager::GetItemData(ItemID).ItemType == EGCInvItemType::Equipment;
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AGCGrabbableItem, Amount))
		{
			return bSuper && !bIsEquipment;
		}
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AGCGrabbableItem, bAutoEquip))
		{
			return bSuper && bIsEquipment;
		}
	}

	return bSuper;
}
#endif

void AGCGrabbableItem::UpdateActor(const uint8 ItemsLeft)
{
	if (ItemsLeft < 1)
	{
		SetEnabled(false);
		return;
	}

	const float VisualsPerItem = Meshes.Num() / Amount;
	int32 AmountToRemove = FMath::RoundToInt(VisualsPerItem * (Amount - ItemsLeft));

	while (AmountToRemove > 0)
	{
		if (MeshComponent->RemoveInstance(FMath::RandRange(0, MeshComponent->GetInstanceCount() - 1)))
		{
			AmountToRemove--;
		}

		if (MeshComponent->GetInstanceCount() == 0)
		{
			SetEnabled(false);
			break;
		}
	}
}

void AGCGrabbableItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	MeshComponent->ClearInstances();
	URCRuntimeLibrary::SetPrimitiveCollision(MeshComponent, Collision);
	
	const FGCInvItemData Data = UGCInventoryManager::GetItemData(ItemID);
	ItemName = Data.DisplayName;
	if (Data.PreviewMesh)
	{
		MeshComponent->SetStaticMesh(Data.PreviewMesh.LoadSynchronous());
		for (int32 i = 0; i < Data.PreviewMaterials.Num(); i++)
		{
			MeshComponent->SetMaterial(i, Data.PreviewMaterials[i].LoadSynchronous());
		}
		
		TArray<FTransform> Transforms{ FTransform::Identity };
		Transforms.Append(Meshes);
	
		for (const FTransform& Instance : Transforms)
		{
			MeshComponent->AddInstance(Instance);
		}
	}

	if (Data.ItemType == EGCInvItemType::Equipment)
	{
		Amount = 1;
	}
}

bool AGCGrabbableItem::GetInteractionInfo_Implementation(FText& DisplayName)
{
	DisplayName = ItemName;
	if (DisplayName.IsEmptyOrWhitespace()) { DisplayName = FText::FromName(*ItemID); }

	return bEnabled && !IsHidden() && GetActorEnableCollision();
}

void AGCGrabbableItem::OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult)
{
	if (!UGCInventoryManager::GetItemData(ItemID).IsValidData() || Amount < 1)
	{
		SetEnabled(false);
		return;
	}
	
	if (UGCInventoryManager* InvManager = UGCInventoryManager::Get(this))
	{
		int32 Overflow;
		InvManager->AddItemToInventory(ItemID, Amount, {}, bAutoEquip, true, false, Overflow);
		UpdateActor(Overflow);
	}
}
