// Copyright (C) RedCraft86. All Rights Reserved.

#include "MeshGen/GTMeshGenBase.h"
#include "Algo/RandomShuffle.h"
#if WITH_EDITOR
#include "AssetToolsModule.h"
#include "EditorDirectories.h"
#include "PhysicsEngine/BodySetup.h"
#include "ProceduralMeshConversion.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "AssetRegistry/AssetRegistryModule.h"
#endif

AGTMeshGenBase::AGTMeshGenBase() : bRealtimeConstruction(true)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
	bRunConstructionScriptOnDrag = true;
#endif
}

template <typename T>
T* AGTMeshGenBase::AddComponent()
{
	if (T* GeneratedComp = NewObject<T>(this))
	{
		GeneratedComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		GeneratedComp->OnComponentCreated();
		GeneratedComp->RegisterComponent();
		GeneratedComp->SetMobility(EComponentMobility::Movable);
		GeneratedComp->AttachToComponent(GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform);
		
		return GeneratedComp;
	}

	return nullptr;
}

void AGTMeshGenBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (bRealtimeConstruction || FApp::IsGame())
	{
		Construct();
	}
}

TArray<int32> AGTMeshGenBase::LoopMeshArray(const TArray<FTransformMeshData>& Sample, const EGeneratorLoopMode Mode, const uint8 Amount)
{
	if (Sample.IsEmpty() || Amount <= 0) return {};
	const TArray<FTransformMeshData> FilteredSample = Sample.FilterByPredicate([](const FTransformMeshData& Elem)->bool
	{
		return Elem.IsValidData();
	});

	if (FilteredSample.IsEmpty())
		return {};

	TArray<int32> RetVal;
	RetVal.Reserve(Amount);
	while (RetVal.Num() < Amount)
	{
		for (int i = 0; i < FilteredSample.Num(); i++)
		{
			RetVal.Add(i);
			if (RetVal.Num() >= Amount)
				break;
		}
	}

	switch (Mode)
	{
	case EGeneratorLoopMode::Reverse:
		Algo::Reverse(RetVal);
		break;

	case EGeneratorLoopMode::Random:
		Algo::RandomShuffle(RetVal);
		break;

	default: break;
	}

	return RetVal;
}

#if WITH_EDITOR
void AGTProcMeshBase::CreateStaticMesh()
{
#if WITH_EDITORONLY_DATA
	for (const int32 i : EditorSections) ProceduralMesh->ClearMeshSection(i);
	
	FString DefaultPath;
	const FString DefaultDirectory = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::NEW_ASSET);
	FPackageName::TryConvertFilenameToLongPackageName(DefaultDirectory, DefaultPath);
	if (DefaultPath.IsEmpty()) DefaultPath = TEXT("/Game/Meshes");
	FString PackageName = DefaultPath / TEXT("ProcMesh");
	
	FString Name;
	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackageName, TEXT(""), PackageName, Name);

	const TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget = SNew(SDlgPickAssetPath)
		.Title(NSLOCTEXT("ProceduralMeshComponentDetails", "ConvertToStaticMeshPickName", "Choose New StaticMesh Location"))
		.DefaultAssetPath(FText::FromString(PackageName));

	if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
	{
		FString UserPackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
		FName MeshName(*FPackageName::GetLongPackageAssetName(UserPackageName));
		if (MeshName == NAME_None)
		{
			UserPackageName = PackageName;
			MeshName = *Name;
		}
		
		FMeshDescription MeshDescription = BuildMeshDescription(ProceduralMesh);
		if (MeshDescription.Polygons().Num() > 0)
		{
			UPackage* Package = CreatePackage(*UserPackageName); check(Package);
			UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, MeshName, RF_Public | RF_Standalone);
			StaticMesh->InitResources();
			StaticMesh->SetLightingGuid();

			FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
			SrcModel.BuildSettings.bRecomputeNormals = false;
			SrcModel.BuildSettings.bRecomputeTangents = false;
			SrcModel.BuildSettings.bRemoveDegenerates = false;
			SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
			SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
			SrcModel.BuildSettings.bGenerateLightmapUVs = true;
			SrcModel.BuildSettings.SrcLightmapIndex = 0;
			SrcModel.BuildSettings.DstLightmapIndex = 1;
			StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
			StaticMesh->CommitMeshDescription(0);

			if (!ProceduralMesh->bUseComplexAsSimpleCollision )
			{
				StaticMesh->CreateBodySetup();
				UBodySetup* NewBodySetup = StaticMesh->GetBodySetup();
				NewBodySetup->BodySetupGuid = FGuid::NewGuid();
				NewBodySetup->AggGeom.ConvexElems = ProceduralMesh->ProcMeshBodySetup->AggGeom.ConvexElems;
				NewBodySetup->bGenerateMirroredCollision = false;
				NewBodySetup->bDoubleSidedGeometry = true;
				NewBodySetup->CollisionTraceFlag = CTF_UseDefault;
				NewBodySetup->CreatePhysicsMeshes();
			}

			TMap<int32, UMaterialInterface*> UniqueMaterials;
			const int32 NumSections = ProceduralMesh->GetNumSections();
			for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
			{
				if (EditorSections.Contains(SectionIdx)) continue;
				UniqueMaterials.Add(SectionIdx, ProceduralMesh->GetMaterial(SectionIdx));
			}

			for (const TPair<int32, UMaterialInterface*>& Material : UniqueMaterials)
			{
				StaticMesh->GetStaticMaterials().Add(FStaticMaterial(Material.Value,
						*FString::Printf(TEXT("Section_%d"), Material.Key)));
			}

			StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
			StaticMesh->Build(false);
			StaticMesh->PostEditChange();

			FAssetRegistryModule::AssetCreated(StaticMesh);
		}
	}

	Construct();
#endif
}
#endif
