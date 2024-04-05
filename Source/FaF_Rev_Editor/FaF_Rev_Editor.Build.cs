// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class FaF_Rev_Editor : ModuleRules
{
    public FaF_Rev_Editor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "GTEditor",
                "FaF_Rev"
            }
        );
    }
}