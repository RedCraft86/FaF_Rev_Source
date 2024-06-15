// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class GTEditor : ModuleRules
{
    public GTEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine", 
                "Slate", 
                "SlateCore",
                "InputCore",
                "UnrealEd",
                "Projects",
                "EditorSubsystem",
                "DeveloperSettings",
                "ComponentVisualizers",
                "PropertyEditor",
                "GTRuntime",
                "GTCore",
            }
        );
    }
}