// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

using UnrealBuildTool;

public class DataGraphRuntime : ModuleRules
{
    public DataGraphRuntime(ReadOnlyTargetRules Target) : base(Target)
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
            }
        );
    }
}