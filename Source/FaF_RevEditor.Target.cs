// Copyright (C) RedCraft86. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FaF_RevEditorTarget : TargetRules
{
	public FaF_RevEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.AddRange(new[] {"FaF_Rev", "FaF_RevEditor"});
	}
}
