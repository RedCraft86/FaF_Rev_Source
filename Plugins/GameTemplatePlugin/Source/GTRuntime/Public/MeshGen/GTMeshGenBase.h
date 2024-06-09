// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/PrimitiveData.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Components/SplineMeshComponent.h"
#include "GTMeshGenBase.generated.h"

UENUM(BlueprintType)
enum class EGeneratorLoopMode : uint8
{
	Forward,
	Reverse,
	Random
};

USTRUCT(BlueprintType)
struct GTRUNTIME_API FSplineMeshData : public FStaticMeshProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshProperties", meta = (DisplayPriority = -1))
		TEnumAsByte<ESplineMeshAxis::Type> MeshAxis;

	FSplineMeshData() : MeshAxis(ESplineMeshAxis::X) {}
	FStaticMeshProperties operator*() const { return {Mesh, Materials, bCastShadows}; }
};

UCLASS(Abstract)
class GTRUNTIME_API AGTMeshGenBase : public AActor
{
	GENERATED_BODY()

public:

	AGTMeshGenBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

protected:

	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		bool bRealtimeConstruction;
	
	UFUNCTION(CallInEditor, Category = "Tools")
		virtual void Construct() { EventConstruct(); }
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Construct")
		void EventConstruct();
	
	template<typename T = UStaticMeshComponent>
	T* AddComponent();
	
	virtual void OnConstruction(const FTransform& Transform) override;

public: // Statics
	
	static TArray<int32> LoopMeshArray(const TArray<FTransformMeshData>& Sample, const EGeneratorLoopMode Mode, const uint8 Amount);
};

UCLASS(Abstract)
class GTRUNTIME_API AGTProcMeshBase : public AGTMeshGenBase
{
	GENERATED_BODY()

public:

	AGTProcMeshBase()
	{
		ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("MeshComponent");
		ProceduralMesh->SetupAttachment(GetRootComponent());
		ProceduralMesh->bAffectDynamicIndirectLighting = false;
		ProceduralMesh->bUseComplexAsSimpleCollision = false;
		ProceduralMesh->SetCastShadow(false);
	}

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UProceduralMeshComponent> ProceduralMesh;
	
#if WITH_EDITORONLY_DATA
protected:
	TSet<int32> EditorSections = {1};
#endif
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Tools")
		virtual void CreateStaticMesh();
#endif
};
