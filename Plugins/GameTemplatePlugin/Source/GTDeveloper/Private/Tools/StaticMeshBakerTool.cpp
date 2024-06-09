// Copyright (C) RedCraft86. All Rights Reserved.

#include "Tools/StaticMeshBakerTool.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Engine/StaticMeshActor.h"
#include "EditorDialogLibrary.h"
#include "Data/PrimitiveData.h"

void FStaticMeshBakerTool::ExecuteAction()
{
	if (UEditorActorSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UEditorActorSubsystem>() : nullptr)
	{
		const TArray<AActor*> Actors = Subsystem->GetSelectedLevelActors();
		if (Actors.IsEmpty()) return;

		if (UEditorDialogLibrary::ShowMessage(
			FText::FromString(TEXT("Are you sure you want to Bake Selected Meshes?")),
			FText::FromString(TEXT("Proceeding will gather static mesh components from the selected actors and bake them into individual static mesh actors.")),
			EAppMsgType::OkCancel, EAppReturnType::Cancel, EAppMsgCategory::Info) != EAppReturnType::Ok) return;

		const EAppReturnType::Type RetType = UEditorDialogLibrary::ShowMessage(
			FText::FromString(TEXT("Do you want to destroy source actors?")),
			FText::FromString(TEXT("By clicking 'Yes' source actors will be destroyed, otherwise, they will be kept.\nDefault: Yes")),
			EAppMsgType::YesNoCancel, EAppReturnType::Yes, EAppMsgCategory::Info);

		if (RetType == EAppReturnType::Cancel) return;
		const bool bDestroySourceActors = RetType == EAppReturnType::Yes;

		// Inner scope
		{
			const FScopedTransaction Transaction(NSLOCTEXT("GTDeveloper", "BakeStaticMeshes", "Bake Static Meshes"));
		
			for (AActor* Actor : Actors)
			{
				if (!Actor) continue;
				TArray<UStaticMeshComponent*> MeshCompArray;
				Actor->GetComponents<UStaticMeshComponent>(MeshCompArray);
				if (MeshCompArray.IsEmpty()) continue;

				int32 Idx = 0;
				for (const UStaticMeshComponent* Comp : MeshCompArray)
				{
					if (!Comp || Comp->IsEditorOnly() || !Comp->GetStaticMesh()) continue;
					if (const UInstancedStaticMeshComponent* InstComp = Cast<UInstancedStaticMeshComponent>(Comp))
					{
						for (int i = 0; i < InstComp->GetNumInstances(); i++)
						{
							FTransform T;
							if (InstComp->GetInstanceTransform(i, T, true))
							{
								AStaticMeshActor* NewActor = Cast<AStaticMeshActor>(Subsystem->SpawnActorFromClass(
								   AStaticMeshActor::StaticClass(), T.GetTranslation(), T.GetRotation().Rotator()));

								NewActor->SetActorScale3D(T.GetScale3D());
								FStaticMeshProperties Prop = UPrimitiveDataLibrary::GetStaticMeshProperties(Comp);
								UPrimitiveDataLibrary::SetStaticMeshProperties(NewActor->GetStaticMeshComponent(), Prop);

								Subsystem->SetActorSelectionState(NewActor, true);
								NewActor->SetActorLabel(FString::Printf(TEXT("%s_%d"), *Comp->GetStaticMesh()->GetName(), i + 1));
								NewActor->SetFolderPath(Actor->GetFolderPath().IsNone() ? TEXT("Baked")
									: *(Actor->GetFolderPath().ToString() / TEXT("Baked")));
							}
						}
					}
					else
					{
						AStaticMeshActor* NewActor = Cast<AStaticMeshActor>(Subsystem->SpawnActorFromClass(
						   AStaticMeshActor::StaticClass(), Comp->GetComponentLocation(), Comp->GetComponentRotation()));

						NewActor->SetActorScale3D(Comp->GetComponentScale());
						FStaticMeshProperties Prop = UPrimitiveDataLibrary::GetStaticMeshProperties(Comp);
						UPrimitiveDataLibrary::SetStaticMeshProperties(NewActor->GetStaticMeshComponent(), Prop);

						Subsystem->SetActorSelectionState(NewActor, true);
						NewActor->SetActorLabel(FString::Printf(TEXT("%s_%d"), *Comp->GetStaticMesh()->GetName(), Idx + 1));
						NewActor->SetFolderPath(Actor->GetFolderPath().IsNone() ? TEXT("Baked")
							: *(Actor->GetFolderPath().ToString() / TEXT("Baked")));
					}

					Idx++;
				}
				
				Subsystem->SetActorSelectionState(Actor, false);
			}

			if (bDestroySourceActors)
			{
				Subsystem->DestroyActors(Actors);
			}
		}
	}
}
