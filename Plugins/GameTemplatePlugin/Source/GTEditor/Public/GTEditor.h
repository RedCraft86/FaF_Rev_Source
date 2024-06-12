﻿// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#define REGISTER_CLASS_CUSTOMIZATION(Module, Class, Customization) Module->RegisterCustomClassLayout(Class::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&Customization::MakeInstance));
#define UNREGISTER_CLASS_CUSTOMIZATION(Module, Class) Module->UnregisterCustomClassLayout(Class::StaticClass()->GetFName());

#define REGISTER_STRUCT_CUSTOMIZATION(Module, Property, Customization) Module->RegisterCustomPropertyTypeLayout(Property::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&Customization::MakeInstance));
#define UNREGISTER_STRUCT_CUSTOMIZATION(Module, Property) Module->UnregisterCustomPropertyTypeLayout(Property::StaticStruct()->GetFName());

class FGTEditorModule final : public IModuleInterface
{
public:
    
	void StartupModule() override;
	void ShutdownModule() override;

private:
	
	TSet<TSharedPtr<class IAssetTypeActions>> AssetTypeActions;
};