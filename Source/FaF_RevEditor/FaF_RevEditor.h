// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

class FFaF_RevEditorModule final : public IModuleInterface
{
public:
    
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
private:
	
    TSet<TSharedPtr<class IAssetTypeActions>> AssetTypeActions;
};
