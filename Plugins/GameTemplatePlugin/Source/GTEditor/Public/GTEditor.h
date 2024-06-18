// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#define REGISTER_CLASS_CUSTOMIZATION(Class, Customization) PropertyModule->RegisterCustomClassLayout(Class::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&Customization::MakeInstance));
#define UNREGISTER_CLASS_CUSTOMIZATION(Class) PropertyModule->UnregisterCustomClassLayout(Class::StaticClass()->GetFName());

#define REGISTER_STRUCT_CUSTOMIZATION_DIRECT(Property, Customization) PropertyModule->RegisterCustomPropertyTypeLayout(Property->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&Customization::MakeInstance));
#define UNREGISTER_STRUCT_CUSTOMIZATION_DIRECT(Property) PropertyModule->UnregisterCustomPropertyTypeLayout(Property->GetFName());

#define REGISTER_STRUCT_CUSTOMIZATION(Property, Customization) REGISTER_STRUCT_CUSTOMIZATION_DIRECT(Property::StaticStruct(), Customization)
#define UNREGISTER_STRUCT_CUSTOMIZATION(Property) UNREGISTER_STRUCT_CUSTOMIZATION_DIRECT(Property::StaticStruct())

#define REGISTER_STRUCT_CUSTOMIZATION_INHERITED(Type, Customization) \
	if (ScriptStruct == Type::StaticStruct() || (ScriptStruct->IsChildOf(Type::StaticStruct()) && ScriptStruct->HasMetaData(TEXT("ApplyBaseStructCustomization")))) \
	{ REGISTER_STRUCT_CUSTOMIZATION_DIRECT(ScriptStruct, Customization) }

#define UNREGISTER_STRUCT_CUSTOMIZATION_INHERITED(Type) \
	if (ScriptStruct == Type::StaticStruct() || (ScriptStruct->IsChildOf(Type::StaticStruct()) && ScriptStruct->HasMetaData(TEXT("ApplyBaseStructCustomization")))) \
	{ UNREGISTER_STRUCT_CUSTOMIZATION_DIRECT(ScriptStruct) }

#define REGISTER_VISUALIZER(Component, Visualizer) \
	{ \
		TSharedPtr<FComponentVisualizer> VisualizerInstance = Visualizer::MakeInstance(); \
		GUnrealEd->RegisterComponentVisualizer(Component::StaticClass()->GetFName(), VisualizerInstance); \
		VisualizerInstance->OnRegister(); \
	}
#define UNREGISTER_VISUALIZER(Component) GUnrealEd->UnregisterComponentVisualizer(Component::StaticClass()->GetFName());

class FGTEditorModule final : public IModuleInterface
{
public:
    
	void StartupModule() override;
	void ShutdownModule() override;

private:
	
	TSet<TSharedPtr<class IAssetTypeActions>> AssetTypeActions;
};
