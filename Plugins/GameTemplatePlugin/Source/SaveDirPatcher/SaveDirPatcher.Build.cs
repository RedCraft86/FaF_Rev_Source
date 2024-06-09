// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;

public class SaveDirPatcher : ModuleRules
{
	public SaveDirPatcher(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.Add("Core");
	}
}
