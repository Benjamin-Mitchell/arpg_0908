// Copyright Tinzie Games

using UnrealBuildTool;
using System.Collections.Generic;

public class arpg_0908Target : TargetRules
{
	public arpg_0908Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		
		ExtraModuleNames.AddRange( new string[] { "arpg_0908" } );
	}
}
