// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class FaF_RevEditor : ModuleRules
{
    public FaF_RevEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "InputCore",
                "UnrealEd", 
                "Engine",
                "AssetTools",
                "DeveloperSettings",
                "PulldownBuilder",
                "GTEditor", 
                "FaF_Rev"
            }
        );
    }
}