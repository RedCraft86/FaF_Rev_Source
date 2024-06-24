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

void AInventoryItemActor::FillAmount(const uint8 InAmount)
{
	if (InAmount != 0)
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
	Args.Append(MetaInteractArgs);
	Args.Add(TEXT("Amount"), Amount);
	Args.Add(TEXT("Item"), ItemData->DisplayName);
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
#if WITH_EDITOR
	if (MeshInstances.IsEmpty()) MeshInstances.Add(FTransform::Identity);
	if (ItemData)
	{
		for (const TPair<FName, FString>& Meta : ItemData->DefaultMetadata)
		{
			if (!Metadata.Contains(Meta.Key)) Metadata.Add(Meta);
		}
	}
	for (const FName Key : NativeItemKeys::All)
	{
		if (!Metadata.Contains(Key)) Metadata.Add(Key);
	}
#endif
	
	if (!ItemData) return;
	const FTransformMeshData MeshData = ItemData->GetMeshData(Metadata);
	if (MeshData.IsValidData() && Amount > 0)
	{
		InstancedStaticMesh->AddInstances(MeshInstances, false);
		UPrimitiveDataLibrary::SetStaticMeshProperties(InstancedStaticMesh, MeshData);
	}
}

void AInventoryItemActor::BeginPlay()
{
	Super::BeginPlay();
	for (const TPair<FName, FString>& Meta : Metadata)
	{
		if (!Meta.Key.IsNone() && !Meta.Value.IsEmpty())
		{
			MetaInteractArgs.Add(TEXT("m") + Meta.Key.ToString(), FText::FromString(Meta.Value));
		}
	}
}
