#include "EditorCore.h"

#include "UnrealEd.h"
#include "PropertyEditorModule.h"
#include "ComponentVisualization/PlayerSensingVisualizer.h"
#include "ComponentVisualization/ElectronicActorVisualizer.h"
#include "ComponentVisualization/TaskActorManagerVisualizer.h"
#include "DetailsCustomization/PlayerCharacterCustomization.h"
#include "DetailsCustomization/MovableNPCCustomization.h"
#include "DetailsCustomization/EnemyAICustomization.h"
#include "Player/GCPlayerCharacter.h"
#include "Characters/EnemyAIBase.h"
#include "Characters/NPCBase.h"

#define LOCTEXT_NAMESPACE "FEditorCoreModule"

void FEditorCoreModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyModule.RegisterCustomClassLayout(AGCPlayerCharacter::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FGCPlayerCharacterCustomization::MakeInstance));

	PropertyModule.RegisterCustomClassLayout(ANPCBase::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FGCMovableNPCCustomization::MakeInstance));

	PropertyModule.RegisterCustomClassLayout(AEnemyAIBase::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FGCEnemyAICustomization::MakeInstance));

	if (GUnrealEd)
	{
		//RegisterVisualizer(UGCPlayerSensingComponent::StaticClass()->GetFName(), FPlayerSensingVisualizer::MakeInstance());
		RegisterVisualizer(UGCTaskActorManagerComponent::StaticClass()->GetFName(), FTaskActorManagerVisualizer::MakeInstance());
		RegisterVisualizer(UGCElectronicVisualizer::StaticClass()->GetFName(), FElectronicActorVisualizer::MakeInstance());
	}
}

void FEditorCoreModule::ShutdownModule()
{
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->UnregisterCustomClassLayout(AGCPlayerCharacter::StaticClass()->GetFName());
		PropertyModule->UnregisterCustomClassLayout(ANPCBase::StaticClass()->GetFName());
	}

	if (GUnrealEd)
	{
		//GUnrealEd->UnregisterComponentVisualizer(UGCPlayerSensingComponent::StaticClass()->GetFName());
		GUnrealEd->UnregisterComponentVisualizer(UGCTaskActorManagerComponent::StaticClass()->GetFName());
		GUnrealEd->UnregisterComponentVisualizer(UGCElectronicVisualizer::StaticClass()->GetFName());
	}
}

void FEditorCoreModule::RegisterVisualizer(const FName& Name, const TSharedPtr<FComponentVisualizer>& Visualizer)
{
	GUnrealEd->RegisterComponentVisualizer(Name, Visualizer);
	Visualizer->OnRegister();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FEditorCoreModule, EditorCore)
