// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

using UnrealBuildTool;
 
public class RCDebugging : ModuleRules
{
	public RCDebugging(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			}
		);
	}
}