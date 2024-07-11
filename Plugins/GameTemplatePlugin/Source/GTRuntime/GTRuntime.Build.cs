// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class GTRuntime : ModuleRules
{
    public GTRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new[] {
                Path.Combine(PluginDirectory,"Source/GTRuntime/Public/Data/"),
                Path.Combine(PluginDirectory,"Source/GTRuntime/Public/Framework/"),
                Path.Combine(PluginDirectory,"Source/GTRuntime/Public/Gameplay/"),
                Path.Combine(PluginDirectory,"Source/GTRuntime/Public/Widget/"),
            }
        );

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UMG",
                "Slate",
                "SlateCore",
                "MovieScene",
                "LevelSequence",
                "GameplayTags",
                "StructUtils",
                "GeometryCore",
                "ApplicationCore",
                "NavigationSystem",
                "DeveloperSettings",
                "ProceduralMeshComponent",
                "DataGraphRuntime",
                "ExpressiveText",
                "GTCore"
            }
        );
        
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new[]
            {
                "AssetRegistry",
                "AssetTools",
                "UnrealEd"
            });
        }
    }
}