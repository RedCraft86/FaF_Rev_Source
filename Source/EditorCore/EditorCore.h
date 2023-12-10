#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEditorCoreModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    static void RegisterVisualizer(const FName& Name, const TSharedPtr<class FComponentVisualizer>& Visualizer);
};
