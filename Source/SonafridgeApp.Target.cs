// Sonafridge 2022

using UnrealBuildTool;
using System.Collections.Generic;

public class SonafridgeAppTarget : TargetRules
{
	public SonafridgeAppTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("SonafridgeApp");
	}
}
