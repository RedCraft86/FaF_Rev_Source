﻿// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

using UnrealBuildTool;

public class EditorCore : ModuleRules
{
    public EditorCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "PropertyEditor",
                "GameCore"
            }
        );
    }
}