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

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(SceneRoot);
	
	InstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedStaticMesh");
	InstancedStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	InstancedStaticMesh->SetupAttachment(SceneRoot);

	InteractionInfo.Label = INVTEXT("Take");
	BoxCollision.SetAllResponses(ECR_Ignore);
	BoxCollision.SetResponse(ECC_Visibility, ECR_Block);
	BoxCollision.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision.SetObjectType(ECC_WorldDynamic);
}

void AInventoryItemActor::FillAmount(const int32 InAmount)
{
	if (InAmount > 0)
	{
		Amount += InAmount;
		SetEnabled(true);
	}
}

void AInventoryItemActor::SetEnabled(const bool bInEnabled)
{
	Super::SetEnabled(Amount > 0 && bInEnabled);
}

bool AInventoryItemActor::GetInteractionInfo_Implementation(FInteractionInfo& Info)
{
	if (!ItemData || Amount <= 0)
		return false;
	
	FFormatNamedArguments Args;
	Args.Add(TEXT("Amount"), Amount);
	Args.Add(TEXT("Name"), ItemData->DisplayName);
	Info.Label = FText::Format(InteractionInfo.Label, Args);
	Info.Icon = InteractionInfo.Icon;
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
		SetEnabled(false);
	}
}

void AInventoryItemActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UPrimitiveDataLibrary::SetPrimitiveCollision(CollisionBox, BoxCollision);
	CollisionBox->SetBoxExtent({
		FMath::Max(FMath::Abs(BoxExtent.X), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Y), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Z), 4.0f)
	});
	
	InstancedStaticMesh->ClearInstances();
	if (MeshInstances.IsEmpty()) MeshInstances.Add(FTransform::Identity);

	if (!ItemData) return;
	if (!ItemData->MeshData.IsEmpty() && Amount > 0)
	{
		InstancedStaticMesh->AddInstances(MeshInstances, false);
		UPrimitiveDataLibrary::SetStaticMeshProperties(InstancedStaticMesh,
			ItemData->GetMeshData(Metadata));
	}

	for (const FName Key : NativeItemKeys::All) { if (!Metadata.Contains(Key)) Metadata.Add(Key); }
	Metadata.Append(ItemData->DefaultMetadata);
}
