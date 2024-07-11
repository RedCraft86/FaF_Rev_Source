// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTEditor.h"
#include "GTEditorStyle.h"
#include "DetailsCustomization/GTActorDetails.h"
#include "DetailsCustomization/InlineCurveDetails.h"
#include "DetailsCustomization/StringPulldownDetails.h"
#include "DetailsCustomization/PrimitiveCollisionDetails.h"
#include "DetailsCustomization/PropertyMetadataDetails.h"
#include "ComponentVisualizers/DebugShapesVisualizer.h"
#include "ComponentVisualizers/VisionConeVisualizer.h"
#include "CustomAssets/GameMusic/MusicDataAssetActions.h"
#include "CustomAssets/Inventory/ItemDataAssetActions.h"
#include "BlueprintEditorModule.h"
#include "AssetToolsModule.h"
#include "UnrealEd.h"

#include "GTActor.h"
#include "GTVolume.h"
#include "Actors/DebugNavPath.h"
#include "MeshGen/GTMeshGenBase.h"
#include "Optimization/SmartCullingVolume.h"
#include "Data/PulldownTypes.h"

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
		REGISTER_CLASS_CUSTOMIZATION(AGTActor, FGTActorDetails)
		REGISTER_CLASS_CUSTOMIZATION(AGTVolume, FGTActorDetails)
		REGISTER_CLASS_CUSTOMIZATION(AGTMeshGenBase, FGTActorDetails)
		REGISTER_CLASS_CUSTOMIZATION(ADebugNavPath, FGTActorDetails)
		REGISTER_CLASS_CUSTOMIZATION(ASmartCullingVolume, FGTActorDetails)
		REGISTER_STRUCT_CUSTOMIZATION(FInlineFloatCurve, FInlineCurveDetails)
		REGISTER_STRUCT_CUSTOMIZATION(FInlineVectorCurve, FInlineCurveDetails)
		REGISTER_STRUCT_CUSTOMIZATION(FInlineColorCurve, FInlineCurveDetails)
		REGISTER_STRUCT_CUSTOMIZATION(FPrimitiveCollision, FPrimitiveCollisionDetails)

		for (TObjectIterator<UScriptStruct> It; It; ++It)
		{
			const UScriptStruct* ScriptStruct = *It; if (!ScriptStruct) continue;
			REGISTER_STRUCT_CUSTOMIZATION_INHERITED(FStringPulldown, FStringPulldownDetails)
		}
	}

	if (GUnrealEd)
	{
		REGISTER_VISUALIZER(UDebugShapesComponent, FDebugShapesVisualizer);
		REGISTER_VISUALIZER(UVisionConeComponent, FVisionConeVisualizer);
	}

	if (const FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools"))
	{
		AssetTypeActions.Add(MakeShareable(new FMusicDataAssetTypeActions()));
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
		UNREGISTER_CLASS_CUSTOMIZATION(AGTActor)
		UNREGISTER_CLASS_CUSTOMIZATION(AGTVolume)
		UNREGISTER_CLASS_CUSTOMIZATION(AGTMeshGenBase)
		UNREGISTER_CLASS_CUSTOMIZATION(ADebugNavPath)
		UNREGISTER_CLASS_CUSTOMIZATION(ASmartCullingVolume)
		UNREGISTER_STRUCT_CUSTOMIZATION(FInlineFloatCurve)
		UNREGISTER_STRUCT_CUSTOMIZATION(FInlineVectorCurve)
		UNREGISTER_STRUCT_CUSTOMIZATION(FInlineColorCurve)
		UNREGISTER_STRUCT_CUSTOMIZATION(FPrimitiveCollision)
		
		for (TObjectIterator<UScriptStruct> It; It; ++It)
		{
			const UScriptStruct* ScriptStruct = *It; if (!ScriptStruct) continue;
			UNREGISTER_STRUCT_CUSTOMIZATION_INHERITED(FStringPulldown)
		}
	}

	if (GUnrealEd)
	{
		UNREGISTER_VISUALIZER(UDebugShapesComponent);
		UNREGISTER_VISUALIZER(UVisionConeComponent);
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