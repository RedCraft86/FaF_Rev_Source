// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class FaF_Rev : ModuleRules
{
	public FaF_Rev(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new[] {
				"FaF_Rev/Framework/",
				"FaF_Rev/Gameplay/",
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"UMG",
				"InputCore", 
				"EnhancedInput",
				"AssetRegistry",
				"GTRuntime",
				"GTCore",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"ExpressiveText"
			}
		);
	}
}
