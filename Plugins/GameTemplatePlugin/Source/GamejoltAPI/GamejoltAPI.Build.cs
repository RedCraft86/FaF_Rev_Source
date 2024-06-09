// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class GamejoltAPI : ModuleRules
{
    public GamejoltAPI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "HTTP",
                "Json",
                "JsonUtilities",
                "DeveloperSettings",
                "GTCore"
            }
        );
    }
}