// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class GTDeveloper : ModuleRules
{
    public GTDeveloper(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UMG",
                "Slate",
                "SlateCore",
                "InputCore",
                "MainFrame",
                "ToolMenus",
                "UnrealEd",
                "Projects",
                "Blutility",
                "UMGEditor",
                "EngineSettings",
                "EditorScriptingUtilities",
                "WebBrowserWidget",
                "GTCore",
                "GTRuntime",
            }
        );
    }
}