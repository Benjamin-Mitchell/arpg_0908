// Copyright Tinzie Games

using UnrealBuildTool;
using System.Collections.Generic;

public class arpg_0908EditorTarget : TargetRules
{
	public arpg_0908EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "arpg_0908" } );
	}
}
