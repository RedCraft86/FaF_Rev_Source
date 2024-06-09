// Copyright (C) RedCraft86. All Rights Reserved.

#include "Tools/StaticMeshInstancerTool.h"
#include "Actors/HierarchicalInstancedStaticMeshActor.h"
#include "Actors/InstancedStaticMeshActor.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Engine/StaticMeshActor.h"
#include "EditorDialogLibrary.h"
#include "Data/PrimitiveData.h"

void FStaticMeshInstancerTool::ExecuteAction()
{
	#define FindMesh(Prop) Meshes.FindByPredicate([Prop](const FInstancerMeshes& Element) -> bool \
		{ \
			return Element.MeshProp.Mesh == Prop.Mesh \
			&& Element.MeshProp.Materials == Prop.Materials \
			&& Element.MeshProp.bCastShadows == Prop.bCastShadows; \
		})
	
	struct FInstancerMeshes
	{
		FString Label = TEXT("Spawned");
		FName FolderPath = NAME_None;
		FStaticMeshProperties MeshProp = {};
		TArray<FTransform> Transforms = {};
	};
	
	if (UEditorActorSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UEditorActorSubsystem>() : nullptr)
	{
		const TArray<AActor*> Actors = Subsystem->GetSelectedLevelActors();
		if (Actors.IsEmpty()) return;

		if (UEditorDialogLibrary::ShowMessage(
			FText::FromString(TEXT("Are you sure you want to Convert Selected Meshes to Instanced Static Meshes?")),
			FText::FromString(TEXT("Proceeding will gather static mesh components from the selected actors and convert them into instanced static mesh actors.")),
			EAppMsgType::OkCancel, EAppReturnType::Cancel, EAppMsgCategory::Info) != EAppReturnType::Ok) return;

		const bool bUseHISM = UEditorDialogLibrary::ShowMessage(FText::FromString(TEXT("Use HISMs?")),
			FText::FromString(TEXT("Hierarchical Instanced Static Mesh will be used instead of Instanced Static Mesh.")),
			EAppMsgType::YesNo, EAppReturnType::No, EAppMsgCategory::Info) == EAppReturnType::Yes;

		// Inner scope
		{
			const FScopedTransaction Transaction(NSLOCTEXT("GTDeveloper", "ConvertMeshesToInstances", "Convert Meshes to Instances"));
		
			FStaticMeshProperties MeshProp;
			TArray<FInstancerMeshes> Meshes;
			for (AActor* Actor : Actors)
			{
				TArray<UStaticMeshComponent*> MeshCompArray;
				Actor->GetComponents<UStaticMeshComponent>(MeshCompArray);
				for (UStaticMeshComponent* Comp : MeshCompArray)
				{
					if (!Comp || Comp->IsEditorOnly()) continue;
					MeshProp = UPrimitiveDataLibrary::GetStaticMeshProperties(Comp);
					if (const UInstancedStaticMeshComponent* InstComp = Cast<UInstancedStaticMeshComponent>(Comp))
					{
						if (FInstancerMeshes* Mesh = FindMesh(MeshProp))
						{
							for (int i = 0; i < InstComp->GetNumInstances(); i++)
							{
								FTransform T;
								if (InstComp->GetInstanceTransform(i, T, true))
								{
									Mesh->Transforms.Add(T);
								}
							}
						}
						else
						{
							FInstancerMeshes NewMesh;
							NewMesh.MeshProp = MeshProp;
							for (int i = 0; i < InstComp->GetNumInstances(); i++)
							{
								FTransform T;
								if (InstComp->GetInstanceTransform(i, T, true))
								{
									NewMesh.Transforms.Add(T);
								}
							}

							NewMesh.Label = FString::Printf(TEXT("%s_%d"),
								*MeshProp.Mesh.GetAssetName(), Meshes.Num() + 1);
						
							NewMesh.FolderPath = Actor->GetFolderPath().IsNone() ? TEXT("Instanced")
								: *Actor->GetFolderPath().ToString();
						
							Meshes.Add(NewMesh);
						}
					}
					else
					{
						if (FInstancerMeshes* Mesh = FindMesh(MeshProp))
						{
							Mesh->Transforms.Add(Comp->GetComponentTransform());
						}
						else
						{
							FInstancerMeshes NewMesh;
							NewMesh.MeshProp = MeshProp;
							NewMesh.Transforms.Add(Comp->GetComponentTransform());
						
							NewMesh.Label = FString::Printf(TEXT("%s_%d"),
								*MeshProp.Mesh.GetAssetName(), Meshes.Num() + 1);
							
							NewMesh.FolderPath = Actor->GetFolderPath().IsNone() ? TEXT("Instanced")
								: *Actor->GetFolderPath().ToString();
						
							Meshes.Add(NewMesh);
						}
					}
				}

				Subsystem->SetActorSelectionState(Actor, false);
			}

			Subsystem->DestroyActors(Actors);

			for (const FInstancerMeshes& Mesh : Meshes)
			{
				if (Mesh.Transforms.IsEmpty()) continue;
				if (Mesh.Transforms.Num() > 1)
				{
					AInstancedStaticMeshActor* ISMActor = Cast<AInstancedStaticMeshActor>(Subsystem->SpawnActorFromClass(
					   bUseHISM ? AHierarchicalInstancedStaticMeshActor::StaticClass() : AInstancedStaticMeshActor::StaticClass(),
					   Mesh.Transforms[0].GetTranslation(), Mesh.Transforms[0].GetRotation().Rotator()));

					ISMActor->SetActorLabel(Mesh.Label);
					ISMActor->SetFolderPath(Mesh.FolderPath);
					Subsystem->SetActorSelectionState(ISMActor, true);
					UPrimitiveDataLibrary::SetStaticMeshProperties(ISMActor->GetStaticMeshComponent(), Mesh.MeshProp);

#if WITH_EDITORONLY_DATA
					ISMActor->Instances.Empty(Mesh.Transforms.Num());
					for (const FTransform& T : Mesh.Transforms)
					{
						ISMActor->Instances.Add({
							Mesh.Transforms[0].InverseTransformRotation(T.GetRotation()),
							Mesh.Transforms[0].InverseTransformPositionNoScale(T.GetTranslation()),
							T.GetScale3D()
						});
					}
#endif

					ISMActor->GetStaticMeshComponent()->ClearInstances();
					ISMActor->GetStaticMeshComponent()->AddInstances(Mesh.Transforms, false, true);
				}
				else
				{
					AStaticMeshActor* SMActor = Cast<AStaticMeshActor>(Subsystem->SpawnActorFromClass(AStaticMeshActor::StaticClass(),
						Mesh.Transforms[0].GetTranslation(), Mesh.Transforms[0].GetRotation().Rotator()));
					SMActor->SetActorScale3D(Mesh.Transforms[0].GetScale3D());
					
					SMActor->SetActorLabel(Mesh.Label);
					SMActor->SetFolderPath(Mesh.FolderPath);
					Subsystem->SetActorSelectionState(SMActor, true);
					UPrimitiveDataLibrary::SetStaticMeshProperties(SMActor->GetStaticMeshComponent(), Mesh.MeshProp);
				}
			}
		}
	}

#undef FindMesh
}
