// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "RCEditor.h"

#include "UnrealEd.h"
#include "BlueprintEditorModule.h"
#include "PropertyMetadataCustomization.h"
#include "ComponentVisualizers/DebugShapesVisualization.h"
#include "DetailsCustomization/RuntimeColorCurveCustomization.h"
#include "DetailsCustomization/GameVersionCustomization.h"
#include "DetailsCustomization/PrimitiveCollisionCustomization.h"
#include "DetailsCustomization/VectorRangeCustomization.h"
#include "DetailsCustomization/RotatorRangeCustomization.h"

#include "RCMathTypes.h"
#include "RCEngineTypes.h"
#include "RCRuntimeTypes.h"
#include "RCPrimitiveTypes.h"
#include "RCDebugShapesVisualizer.h"

DEFINE_LOG_CATEGORY(RCEditor);

#define LOCTEXT_NAMESPACE "FRCEditor"

#define REGISTER_STRUCT_CUSTOMIZATION(Module, Property, Customization) Module.RegisterCustomPropertyTypeLayout(Property::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&Customization::MakeInstance));
#define UNREGISTER_STRUCT_CUSTOMIZATION(Module, Property) Module->UnregisterCustomPropertyTypeLayout(Property::StaticStruct()->GetFName());

void FRCEditorModule::StartupModule()
{
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	BlueprintEditorModule.RegisterVariableCustomization(FProperty::StaticClass(), FOnGetVariableCustomizationInstance::CreateStatic(&FPropertyMetadataCustomization::MakeInstance));
	
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRuntimeLinearColorCurve, FRuntimeColorCustomization);
	REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRCGameVersion, FGameVersionCustomization);
	REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRCPrimitiveCollision, FPrimitiveCollisionCustomization);
	REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRCVectorRange, FVectorRangeCustomization);
	REGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRCRotatorRange, FRotatorRangeCustomization);

	if (GUnrealEd)
	{
		const TSharedPtr<FComponentVisualizer> Visualizer = FDebugComponentVisualizer::MakeInstance();
		GUnrealEd->RegisterComponentVisualizer(URCDebugShapeVisualizer::StaticClass()->GetFName(), Visualizer);
		Visualizer->OnRegister();
	}
}

void FRCEditorModule::ShutdownModule()
{
	if (FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet"))
	{
		FDelegateHandle Handle;
		BlueprintEditorModule->UnregisterVariableCustomization(FProperty::StaticClass(), Handle);
		Handle.Reset();
	}
	
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRuntimeLinearColorCurve);
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRCGameVersion);
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRCPrimitiveCollision);
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRCVectorRange);
		UNREGISTER_STRUCT_CUSTOMIZATION(PropertyModule, FRCRotatorRange);
	}

	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(URCDebugShapeVisualizer::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRCEditorModule, RCEditor)