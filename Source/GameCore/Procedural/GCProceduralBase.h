// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCPrimitiveTypes.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "GCProceduralBase.generated.h"

USTRUCT(BlueprintType, DisplayName = "Procedural Static Mesh Properties")
struct GAMECORE_API FGCProceduralStaticMesh : public FRCStaticMeshProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ProceduralStaticMesh")
		FTransform Offset;

	FGCProceduralStaticMesh() : Offset(FTransform::Identity) {}

	friend FArchive& operator<<(FArchive& Ar, FGCProceduralStaticMesh& Properties)
	{
		Ar << Properties.Mesh;
		Ar << Properties.Materials;
		Ar << Properties.bCastShadows;
		Ar << Properties.Offset;
		return Ar;
	}
};

USTRUCT(BlueprintType, DisplayName = "Spline Static Mesh Properties")
struct GAMECORE_API FGCSplineStaticMesh : public FRCStaticMeshProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SplineStaticMesh")
		TEnumAsByte<ESplineMeshAxis::Type> MeshAxis;

	FGCSplineStaticMesh() : MeshAxis(ESplineMeshAxis::X) {}

	friend FArchive& operator<<(FArchive& Ar, FGCSplineStaticMesh& Properties)
	{
		Ar << Properties.Mesh;
		Ar << Properties.Materials;
		Ar << Properties.bCastShadows;
		Ar << Properties.MeshAxis;
		return Ar;
	}
};

UENUM(BlueprintType, DisplayName = "Procedural Mesh Spawn Type")
enum class EGCProceduralSpawnType : uint8
{
	ForwardOrder,
	ReverseOrder,
	RandomOrder,
};

UCLASS(Abstract, Blueprintable, DisplayName = "Procedural Actor Base")
class GAMECORE_API AGCProceduralBase : public AActor
{
	GENERATED_BODY()

public:
	
	AGCProceduralBase();

	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		bool bRealtimeConstruction;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Construct")
		void EventConstruct();

	UFUNCTION(CallInEditor, Category = "Tools")
		virtual void Construct() { EventConstruct(); }

#if WITH_EDITOR
	/** Convert the selected Generated Mesh into a single Static Mesh. */
	UFUNCTION(CallInEditor, Category = "Tools")
		static void MakeStaticMesh() { FGlobalTabmanager::Get()->TryInvokeTab(FName("MergeActors")); }
#endif

protected:
	
	virtual void OnConstruction(const FTransform& Transform) override;
};
