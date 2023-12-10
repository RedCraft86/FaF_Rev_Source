// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

using UnrealBuildTool;
using System.IO;
 
public class RCRuntime : ModuleRules
{
	public RCRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				Path.Combine(ModuleDirectory, "Public/"),
                Path.Combine(ModuleDirectory, "Public/Data/"),
				Path.Combine(ModuleDirectory, "Public/Libraries/"),
			}
		);
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"Engine",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"UMG",
				"Slate",
				"SlateCore",
				"InputCore",
				"MovieScene",
				"LevelSequence",
				"EngineSettings",
				"ApplicationCore",
				"DeveloperSettings",
				"GameplayTags",
				"StructUtils"
			}
		);
	}
}