// Sonafridge 2022

using UnrealBuildTool;
using System.Collections.Generic;

public class SonafridgeAppEditorTarget : TargetRules
{
	public SonafridgeAppEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("SonafridgeApp");
	}
}
