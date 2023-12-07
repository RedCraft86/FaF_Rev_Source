// Copyright (C) Tayzar Linn. All Rights Reserved.

using UnrealBuildTool;
 
public class RCEditor : ModuleRules
{
	public RCEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"RCRuntime",
				"RCDebugging"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"Kismet",
				"PropertyEditor",
				"EditorStyle",
				"InputCore",
				"EditorSubsystem",
				"DeveloperSettings",
			}
		);
	}
}