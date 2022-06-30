// Sonafridge 2022

using UnrealBuildTool;

public class SonafridgeEditor : ModuleRules
{
    public SonafridgeEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                // ... add public include paths required here ...
            }
        );


        PrivateIncludePaths.AddRange(
            new string[]
            {
                // ... add other private include paths required here ...
            }
        );


        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core"
                // ... add other public dependencies that you statically link with here ...
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "Projects",
                "InputCore",
                "UnrealEd",
                "ToolMenus",
                "CoreUObject",
                "Engine",
                "SlateCore",
                "Slate",
                "SlateCore",
                "Blutility",
                "Synthesis",
                "UMG",
                "Sonafridge"
                // ... add private dependencies that you statically link with here ...	
            }
        );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
        );
    }
}