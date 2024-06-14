// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class GTCore : ModuleRules
{
    public GTCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore",
                "MovieScene",
                "LevelSequence",
                "JsonUtilities",
                "Json",
                "UMG",
                "LowEntryExtendedStandardLibrary"
            }
        );
    }
}