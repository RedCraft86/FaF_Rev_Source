// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FaF_RevTarget : TargetRules
{
	public FaF_RevTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange( new string[] { "GameCore" } );
	}
}
