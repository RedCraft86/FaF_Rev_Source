// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class FaF_Rev : ModuleRules
{
	public FaF_Rev(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new[] {
				"FaF_Rev/",
				"FaF_Rev/Data",
				"FaF_Rev/Framework/",
				"FaF_Rev/Gameplay/",
				"FaF_Rev/UserWidgets/",
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"UMG",
				"RHI",
				"Slate",
				"SlateCore",
				"InputCore",
				"PhysicsCore",
				"GameplayTags",
				"EnhancedInput",
				"AssetRegistry",
				"LevelSequence",
				"EngineSettings",
                "CinematicCamera",
                "NavigationSystem",
				"DeveloperSettings",
				"DataGraphRuntime",
				"PulldownStruct",
				"ExpressiveText",
				"StructUtils",
				"Narrative",
				"SMSystem",
				"GTRuntime",
				"GTCore",
			}
		);
	}
}
