// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FaF_RevTarget : TargetRules
{
	public FaF_RevTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("FaF_Rev");
	}
}
