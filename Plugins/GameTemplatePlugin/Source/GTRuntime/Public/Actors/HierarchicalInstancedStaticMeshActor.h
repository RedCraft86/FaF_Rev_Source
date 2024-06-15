// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "InstancedStaticMeshActor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "HierarchicalInstancedStaticMeshActor.generated.h"

UCLASS()
class GTRUNTIME_API AHierarchicalInstancedStaticMeshActor : public AInstancedStaticMeshActor
{
	GENERATED_BODY()

public:

	AHierarchicalInstancedStaticMeshActor(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer.SetDefaultSubobjectClass<UHierarchicalInstancedStaticMeshComponent>(StaticMeshComponentName))
    {
    	bCanBeInCluster = GetClass() == AHierarchicalInstancedStaticMeshActor::StaticClass();
    }
};
