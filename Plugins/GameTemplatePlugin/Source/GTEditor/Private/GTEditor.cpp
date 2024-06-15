// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTEditor.h"
#include "GTEditorStyle.h"
#include "DetailsCustomization/GTActorDetails.h"
#include "DetailsCustomization/InlineCurveDetails.h"
#include "DetailsCustomization/PrimitiveCollisionDetails.h"
#include "DetailsCustomization/PropertyMetadataDetails.h"
#include "ComponentVisualizers/DebugShapesVisualizer.h"
#include "CustomAssets/Inventory/ItemDataAssetActions.h"
#include "BlueprintEditorModule.h"
#include "AssetToolsModule.h"
#include "UnrealEd.h"

#include "GTActor.h"
#include "GTVolume.h"
#include "MeshGen/GTMeshGenBase.h"
#include "Optimization/SmartCullingVolume.h"

#define LOCTEXT_NAMESPACE "GTEditor"

void FGTEditorModule::StartupModule()
{
	FGTEditorStyle::Initialize();
	
	if (FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::LoadModulePtr<FBlueprintEditorModule>("Kismet"))
	{
		BlueprintEditorModule->RegisterVariableCustomization(FProperty::StaticClass(),
			FOnGetVariableCustomizationInstance::CreateStatic(&FPropertyMetadataDetails::MakeInstance));
	}
	
	if (FPropertyEditorModule* PropertyModule = FModuleManager::LoadModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		REGISTER_CLASS_CUSTOMIZATION(PropertyModule, AGTActor, FGTActorDetails);
		REGISTER_CLASS_CUSTOMIZATION(PropertyModule, AGTVolume, FGTActorDetails);
		REGISTER_CLASS_CUSTOMIZATION(PropertyModule, AGTMeshGenBase, FGTActorDetails);
		REGISTER_CLASS_CUSTOMIZATION(PropertyModule, ASmartCullingVolume, FGTActorDetails);
		REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FInlineFloatCurve, FInlineCurveDetails);
		REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FInlineVectorCurve, FInlineCurveDetails);
		REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FInlineColorCurve, FInlineCurveDetails);
		REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FPrimitiveCollision, FPrimitiveCollisionDetails);
	}

	if (GUnrealEd)
	{
		REGISTER_VISUALIZER(UDebugShapesComponent, FDebugShapesVisualizer);
	}

	if (const FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools"))
	{
		AssetTypeActions.Add(MakeShareable(new FItemDataAssetTypeActions()));
		for (const TSharedPtr<IAssetTypeActions>& Action : AssetTypeActions)
		{
			AssetToolsModule->Get().RegisterAssetTypeActions(Action.ToSharedRef());
		}
	}
}

void FGTEditorModule::ShutdownModule()
{
	FGTEditorStyle::Shutdown();
	
	if (FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet"))
	{
		const FDelegateHandle Handle;
		BlueprintEditorModule->UnregisterVariableCustomization(FProperty::StaticClass(), Handle);
	}
	
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, AGTActor);
		UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, AGTVolume);
		UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, AGTMeshGenBase);
		UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, ASmartCullingVolume);
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FInlineFloatCurve);
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FInlineVectorCurve);
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FInlineColorCurve);
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FPrimitiveCollision);
	}

	if (GUnrealEd)
	{
		UNREGISTER_VISUALIZER(UDebugShapesComponent);
	}

	if (const FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		for (const TSharedPtr<IAssetTypeActions>& Action : AssetTypeActions)
		{
			if (Action.IsValid()) AssetToolsModule->Get().UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FGTEditorModule, GTEditor)