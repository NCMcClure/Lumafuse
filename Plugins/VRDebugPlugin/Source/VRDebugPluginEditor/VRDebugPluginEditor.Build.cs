// Copyright 2019 DownToCode. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class VRDebugPluginEditor : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string PluginPath
	{
		get { return Path.Combine(PluginDirectory, "Source/"); }
	}

	public VRDebugPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add(Path.Combine(PluginPath, "VRDebugPluginEditor/Private"));
		PublicIncludePaths.Add(Path.Combine(PluginPath, "VRDebugPluginEditor/Public"));
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
				"InputCore",
				"EngineSettings",
                "Engine",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "VRDebugPlugin",
				"Projects",
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
