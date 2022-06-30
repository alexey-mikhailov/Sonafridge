// Sonafridge 2022

using UnrealBuildTool;

public class Sonafridge : ModuleRules
{
    public Sonafridge(ReadOnlyTargetRules Target) : base(Target)
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
                "InputCore",
                "Engine",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "Synthesis",
                "UMG"
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