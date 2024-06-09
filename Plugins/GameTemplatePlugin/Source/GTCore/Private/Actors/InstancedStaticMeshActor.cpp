// Copyright (C) RedCraft86. All Rights Reserved.

#include "Actors/InstancedStaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"

#if WITH_EDITOR
#include "Misc/MapErrors.h"
#include "Misc/UObjectToken.h"
#include "PhysicsEngine/BodySetup.h"
#include "UObject/FrameworkObjectVersion.h"
#include "StaticMeshComponentLODInfo.h"
#include "Engine/OverlapResult.h"
#endif

#define LOCTEXT_NAMESPACE "InstancedStaticMeshActor"
FName AInstancedStaticMeshActor::StaticMeshComponentName(TEXT("StaticMeshComponent0"));

AInstancedStaticMeshActor::AInstancedStaticMeshActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetCanBeDamaged(false);

	StaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(StaticMeshComponentName);
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	StaticMeshComponent->Mobility = EComponentMobility::Static;
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->bUseDefaultCollision = true;
	RootComponent = StaticMeshComponent;

#if WITH_EDITOR
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshFinder.Succeeded()) StaticMeshComponent->SetStaticMesh(MeshFinder.Object);
	
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MaterialFinder.Succeeded()) StaticMeshComponent->SetMaterial(0, MaterialFinder.Object);
#endif
	
	bCanBeInCluster = GetClass() == AInstancedStaticMeshActor::StaticClass();
}

void AInstancedStaticMeshActor::SetMobility(const EComponentMobility::Type InMobility) const
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetMobility(InMobility);
	}
}

#if WITH_EDITOR
void AInstancedStaticMeshActor::PostLoad()
{
	Super::PostLoad();
	if (StaticMeshComponent && GetLinkerCustomVersion(FFrameworkObjectVersion::GUID) < FFrameworkObjectVersion::UseBodySetupCollisionProfile)
	{
		if (const UBodySetup* BodySetup = StaticMeshComponent->GetBodySetup())
		{
			if (BodySetup->DefaultInstance.GetCollisionProfileName() != StaticMeshComponent->GetCollisionProfileName())
			{
				StaticMeshComponent->bUseDefaultCollision = false;
			}
		}
	}
}

void AInstancedStaticMeshActor::CheckForErrors()
{
	Super::CheckForErrors();
	FMessageLog MapCheck("MapCheck");
	if (!StaticMeshComponent)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
		MapCheck.Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::Format(LOCTEXT( "MapCheck_Message_StaticMeshComponent", "Static mesh actor {ActorName} has NULL StaticMeshComponent property - please delete" ), Arguments)))
			->AddToken(FMapErrorToken::Create(FMapErrors::StaticMeshComponent));
	}
	else if (StaticMeshComponent->GetStaticMesh() != nullptr)
	{
		TArray<FOverlapResult> Overlaps;
		GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(1.f),
			FCollisionQueryParams(SCENE_QUERY_STAT(CheckForErrors), false, this));

		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.OverlapObjectHandle.IsValid() && Overlap.OverlapObjectHandle != this && Overlap.OverlapObjectHandle.DoesRepresentClass(AInstancedStaticMeshActor::StaticClass())
				&& (Overlap.OverlapObjectHandle.GetLocation() - GetActorLocation()).IsNearlyZero() && Overlap.OverlapObjectHandle.GetRotation() == GetActorRotation())
			{
				AInstancedStaticMeshActor* A = Overlap.OverlapObjectHandle.FetchActor<AInstancedStaticMeshActor>();
				check(A);

				if (A->StaticMeshComponent && (A->StaticMeshComponent->GetStaticMesh() == StaticMeshComponent->GetStaticMesh()) && (A->StaticMeshComponent->GetRelativeScale3D() == StaticMeshComponent->GetRelativeScale3D()))
				{
					FFormatNamedArguments Arguments;
						Arguments.Add(TEXT("ActorName0"), FText::FromString(GetName()));
						Arguments.Add(TEXT("ActorName1"), FText::FromString(A->GetName()));
						MapCheck.Warning()
						->AddToken(FUObjectToken::Create(this))
						->AddToken(FTextToken::Create(FText::Format(LOCTEXT("MapCheck_Message_SameLocation", "{ActorName0} is in the same location as {ActorName1}"), Arguments)))
						->AddToken(FMapErrorToken::Create(FMapErrors::SameLocation));
				}
			}
		}

		const int32 NumLODs = StaticMeshComponent->GetStaticMesh()->GetNumLODs();
		for (int32 CurLODIndex = 0; CurLODIndex < NumLODs; ++CurLODIndex)
		{
			const FStaticMeshLODResources& LODRenderData = StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[CurLODIndex];
			if (StaticMeshComponent->LODData.Num() > CurLODIndex)
			{
				const FStaticMeshComponentLODInfo& ComponentLODInfo = StaticMeshComponent->LODData[CurLODIndex];

				if (ComponentLODInfo.OverrideVertexColors && ComponentLODInfo.OverrideVertexColors->GetNumVertices() != LODRenderData.GetNumVertices())
				{
					FFormatNamedArguments Arguments;
					Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
					Arguments.Add(TEXT("LODIndex"), CurLODIndex);
					Arguments.Add(TEXT("StaticMeshName"), FText::FromString(StaticMeshComponent->GetStaticMesh()->GetName()));

					MapCheck.Warning()
						->AddToken(FUObjectToken::Create(this))
						->AddToken(FTextToken::Create(FText::Format( LOCTEXT("MapCheck_Message_VertexColorsNotMatchOriginalMesh", "{ActorName} (LOD {LODIndex}) has hand-painted vertex colors that no longer match the original StaticMesh ({StaticMeshName})" ), Arguments ) ))
						->AddToken(FMapErrorToken::Create(FMapErrors::VertexColorsNotMatchOriginalMesh));
				}
			}
		}
	}
}

bool AInstancedStaticMeshActor::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
	if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
	{
		Objects.Add(StaticMeshComponent->GetStaticMesh());
	}
	
	return true;
}
#endif

void AInstancedStaticMeshActor::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
#if WITH_EDITOR
	Ar.UsingCustomVersion(FFrameworkObjectVersion::GUID);
#endif
}

#if WITH_EDITOR
void AInstancedStaticMeshActor::LoadedFromAnotherClass(const FName& OldClassName)
{
	Super::LoadedFromAnotherClass(OldClassName);
	if(GetLinkerUEVersion() < VER_UE4_REMOVE_STATICMESH_MOBILITY_CLASSES)
	{
		static FName InterpActor_NAME(TEXT("InterpActor"));
		static FName PhysicsActor_NAME(TEXT("PhysicsActor"));

		if(OldClassName == InterpActor_NAME)
		{
			StaticMeshComponent->Mobility = EComponentMobility::Movable;
			StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
		}
		else if(OldClassName == PhysicsActor_NAME)
		{
			StaticMeshComponent->Mobility = EComponentMobility::Movable;
			StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
			StaticMeshComponent->BodyInstance.bSimulatePhysics = true;

			SetCanBeDamaged(true);
			SetReplicatingMovement(true);
			SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
			bReplicates = true;
		}
	}
}

void AInstancedStaticMeshActor::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property != nullptr)
	{
		if (PropertyChangedEvent.Property->GetFName() == FName(TEXT("bStaticMeshReplicateMovement")))
		{
			SetReplicatingMovement(bStaticMeshReplicateMovement);
			SetReplicates(IsReplicatingMovement());
		}
	}

	const FName TailPropName = PropertyChangedEvent.PropertyChain.GetTail()->GetValue()->GetFName();
	static FName SimulatePhysics_NAME(TEXT("bSimulatePhysics"));
	if (TailPropName == SimulatePhysics_NAME)
	{
		SetCanBeDamaged(StaticMeshComponent->BodyInstance.bSimulatePhysics);
	}
}

void AInstancedStaticMeshActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITORONLY_DATA
	StaticMeshComponent->ClearInstances();
	StaticMeshComponent->AddInstances(Instances, false);
#endif
}
#endif

FString AInstancedStaticMeshActor::GetDetailedInfoInternal() const
{
	return StaticMeshComponent
		? StaticMeshComponent->GetDetailedInfoInternal()
		: TEXT("No_InstancedStaticMeshComponent");
}

void AInstancedStaticMeshActor::BeginPlay()
{
	if (GetLocalRole() == ROLE_Authority && bStaticMeshReplicateMovement)
	{
		bReplicates = false;
		SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
		SetReplicates(true);
	}	

	if (StaticMeshComponent && StaticMeshComponent->BodyInstance.bSimulatePhysics)
	{
		SetPhysicsReplicationMode(StaticMeshPhysicsReplicationMode);
	}

	Super::BeginPlay();
}

#undef LOCTEXT_NAMESPACE