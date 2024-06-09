// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

using UnrealBuildTool;
using System.IO;

public class DataGraphEditor : ModuleRules
{
    public DataGraphEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new[]
            {
                Path.Combine(PluginDirectory, "Source/DataGraphEditor/Public/AssetEditor"),
                Path.Combine(PluginDirectory, "Source/DataGraphEditor/Public/AutoLayout"),
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "UnrealEd",
                "Engine",
                "Slate",
                "SlateCore",
                "Projects",
                "ToolMenus",
                "AssetTools",
                "GraphEditor",
                "ApplicationCore",
                "DeveloperSettings",
                "DataGraphRuntime",
            }
        );
    }
}