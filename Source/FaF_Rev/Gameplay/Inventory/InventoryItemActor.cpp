// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryItemActor.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "InventoryComponent.h"
#include "InventoryItemData.h"
#include "FRGameMode.h"
#include "FRPlayer.h"

AInventoryItemActor::AInventoryItemActor() : ItemData(nullptr), Amount(1), BoxExtent({32.0f})
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
#if WITH_EDITOR
	SceneRoot->bVisualizeComponent = true;
#endif

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(SceneRoot);
	
	InstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedStaticMesh");
	InstancedStaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
	InstancedStaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	InstancedStaticMesh->SetupAttachment(SceneRoot);

	InteractionInfo.Label = INVTEXT("Take");
	CollisionData.SetAllResponses(ECR_Ignore);
	CollisionData.SetResponse(ECC_Visibility, ECR_Block);
	CollisionData.CollisionEnabled = ECollisionEnabled::QueryOnly;
	CollisionData.ObjectType = ECC_WorldDynamic;
}

bool AInventoryItemActor::GetInteractionInfo_Implementation(FInteractionInfo& Info)
{
	if (!ItemData || Amount <= 0)
		return false;
	
	Info = InteractionInfo;
	Info.Label = FText::Format(InteractionInfo.Label,
		TEXT("Name"), ItemData->DisplayName,
		TEXT("Amount"), Amount
	);
	return true;
}

void AInventoryItemActor::OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult)
{
	if (ItemData && Amount > 0)
	{
		Amount = Player->GetGameMode()->Inventory->AddItem(ItemData, Amount, Metadata);
	}
	
	if (Amount <= 0)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	}
}

void AInventoryItemActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UPrimitiveDataLibrary::SetPrimitiveCollision(BoxCollision, CollisionData);
	BoxCollision->SetBoxExtent({
		FMath::Max(FMath::Abs(BoxExtent.X), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Y), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Z), 4.0f)
	});
	
	InstancedStaticMesh->ClearInstances();
	if (MeshInstances.IsEmpty()) MeshInstances.Add(FTransform::Identity);
	if (ItemData && !ItemData->MeshData.IsEmpty() && Amount > 0)
	{
		InstancedStaticMesh->AddInstances(MeshInstances, false);
		UPrimitiveDataLibrary::SetStaticMeshProperties(InstancedStaticMesh,
			ItemData->GetMeshData(Metadata));
	}
}
