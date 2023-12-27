// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCMeshRow.h"
#include "RCRuntimeLibrary.h"

AGCMeshRow::AGCMeshRow()
{
	PrimaryActorTick.bCanEverTick = false;

	SetHidden(false);
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif

	Collision.SetCollisionProfile("BlockAllDynamic");
	MeshCount = 1;
	MeshOffset = FVector{10.0f, 0.0f, 0.0f};
	InstanceTransform = FTransform::Identity;
}

void AGCMeshRow::Construct()
{
	StaticMeshes.Empty(MeshCount);
	for (int i = 0; i < MeshCount; i++)
	{
		if (UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(this))
		{
			MeshComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			MeshComp->OnComponentCreated();
			MeshComp->RegisterComponent();
			MeshComp->SetRelativeTransform(FTransform{
				InstanceTransform.Rotator(),
				InstanceTransform.GetTranslation() + (MeshOffset * i),
				InstanceTransform.GetScale3D()
			});
			
			URCRuntimeLibrary::SetStaticMeshProperties(MeshComp, ReplaceMeshes.Contains(i) ? ReplaceMeshes.FindRef(i) : MeshData);
			URCRuntimeLibrary::SetPrimitiveCollision(MeshComp, Collision);

			MeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			StaticMeshes.Add(MeshComp);
		}
	}
	
	Super::Construct();
}
