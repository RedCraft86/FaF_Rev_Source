// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

using UnrealBuildTool;
 
public class GameCore : ModuleRules
{
	public GameCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[]
			{
				"GameCore/"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"HTTP",
				"Json",
				"RHI",
				"UMG",
				"Slate",
				"SlateCore",
				"JsonUtilities",
				"DeveloperSettings",
				"EngineSettings",
				"AssetRegistry",
				"GameplayTags",
				"StructUtils",
				"InputCore",
				"RenderCore",
				"MovieScene",
				"PhysicsCore",
				"GeometryCore",
				"LevelSequence",
				"EnhancedInput",
				"CinematicCamera",
				"ProceduralMeshComponent",
				"PulldownStruct",
				"ExpressiveText",
				"Narrative",
				"RCRuntime",
			}
		);
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}