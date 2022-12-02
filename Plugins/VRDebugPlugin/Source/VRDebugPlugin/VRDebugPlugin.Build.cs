// Copyright 2019 DownToCode. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class VRDebugPlugin : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string PluginPath
	{
		get { return Path.Combine(PluginDirectory, "Source/"); }
	}

	public VRDebugPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add(Path.Combine(PluginPath, "VRDebugPlugin/Private"));
		PublicIncludePaths.Add(Path.Combine(PluginPath, "VRDebugPlugin/Public"));
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Slate",
                "SlateCore",
				// ... add other public dependencies that you statically link with here ...
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "InputCore",
                "UMG",
                "HeadMountedDisplay",
                "Projects"
				// ... add private dependencies that you statically link with here ...	
			}
			);

        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {   
                "UnrealEd",   
			}
            );
        }

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
