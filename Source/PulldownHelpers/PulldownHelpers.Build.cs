// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

using UnrealBuildTool;

public class PulldownHelpers : ModuleRules
{
    public PulldownHelpers(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                "PulldownHelpers/",
                "PulldownHelpers/Generators",
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "InputCore",
                "EnhancedInput", 
                "DeveloperSettings",
                "PulldownStruct",
                "PulldownBuilder",
                "GameCore"
            }
        );
    }
}