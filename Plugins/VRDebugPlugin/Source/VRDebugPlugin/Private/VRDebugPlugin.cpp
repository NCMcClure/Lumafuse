// Copyright 2019 DownToCode. All Rights Reserved.

#include "VRDebugPlugin.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Core/Public/HAL/PlatformFileManager.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"

#define LOCTEXT_NAMESPACE "FVRDebugPluginModule"

void FVRDebugPluginModule::StartupModule()
{
	DefaultCommands.Add("[Custom]");
	DefaultCommands.Add("DONOTDELETE= ");
	DefaultCommands.Add("");
	DefaultCommands.Add("[VR]");
	DefaultCommands.Add("vr.PixelDensity= ");
	DefaultCommands.Add("vr.HMDVersion= ");
	DefaultCommands.Add("vr.bEnableHMD=");
	DefaultCommands.Add("vr.bEnableStereo= ");
	DefaultCommands.Add("vr.WorldToMetersScale= ");
	DefaultCommands.Add("vr.HiddenAreaMask= ");
	DefaultCommands.Add("vr.SpectatorScreenMode= ");
	DefaultCommands.Add("vr.TrackingOrigin= ");
	DefaultCommands.Add("vr.HeadTracking.Reset= ");
	DefaultCommands.Add("vr.HeadTracking.ResetPosition= ");
	DefaultCommands.Add("vr.HeadTracking.ResetOrientation= ");
	DefaultCommands.Add("vr.HeadTracking.Status= ");
	DefaultCommands.Add("vr.oculus.ShowToolWindow= ");
	DefaultCommands.Add("vr.oculus.Stress.CPU= ");
	DefaultCommands.Add("vr.oculus.Stress.GPU= ");
	DefaultCommands.Add("vr.oculus.Stress.PD= ");
	DefaultCommands.Add("vr.oculus.Stress.Reset= ");
	DefaultCommands.Add("vr.Debug.VisualizeTrackingSensors= ");
	DefaultCommands.Add("vr.EnableMotionControllerLateUpdate= ");
	DefaultCommands.Add("vr.InstancedStereo= ");
	DefaultCommands.Add("vr.MirrorMode= ");
	DefaultCommands.Add("vr.MobileMultiView= ");
	DefaultCommands.Add("vr.MobileMultiView.Direct= ");
	DefaultCommands.Add("vr.MonoscopicFarField= ");
	DefaultCommands.Add("vr.MonoscopicFarFieldMode= ");
	DefaultCommands.Add("vr.MultiView= ");
	DefaultCommands.Add("vr.ODSCapture= ");
	DefaultCommands.Add("vr.RoundRobinOcclusion= ");
	DefaultCommands.Add("vr.SetTrackingOrigin= ");
	DefaultCommands.Add("vr.SteamVR.EnableVRInput= ");
	DefaultCommands.Add("vr.SteamVR.UsePostPresentHandoff= ");
	DefaultCommands.Add("vr.SteamVR.UseVisibleAreaMesh= ");
	DefaultCommands.Add("vr.StereoLayers.bMixLayerPriorities= ");
	DefaultCommands.Add("vr.SwapMotionControllerInput= ");
	DefaultCommands.Add("");
	DefaultCommands.Add("[Net]");
	DefaultCommands.Add("p.NetShowCorrections= ");
	DefaultCommands.Add("net.ForceNetFlush= ");
	DefaultCommands.Add("net.DebugDraw= ");
	DefaultCommands.Add("net.RPC.Debug= ");
	DefaultCommands.Add("net.SimulateConnections= ");
	DefaultCommands.Add("");
	DefaultCommands.Add("[R]");
	DefaultCommands.Add("r.MobileHDR= ");
	DefaultCommands.Add("r.SSR.Quality= ");
	DefaultCommands.Add("r.BloomQuality= ");
	DefaultCommands.Add("r.SetRes= ");
	DefaultCommands.Add("");
	DefaultCommands.Add("[Show]");
	DefaultCommands.Add("show Collision= ");
	DefaultCommands.Add("show ShaderComplexity= ");
	DefaultCommands.Add("show Volumes= ");
	DefaultCommands.Add("show DeferredLighting= ");
	DefaultCommands.Add("show DebugAI= ");
	DefaultCommands.Add("show DirectionalLights= ");
	DefaultCommands.Add("show Lighting= ");
	DefaultCommands.Add("show TemporalAA= ");
	DefaultCommands.Add("show Wireframe= ");
	DefaultCommands.Add("show AntiAliasing= ");
	DefaultCommands.Add("show PostProcessing= ");
	DefaultCommands.Add("show Splines= ");
	DefaultCommands.Add("show SpotLights= ");
	DefaultCommands.Add("show StaticMeshes= ");
	DefaultCommands.Add("show LightComplexity= ");
	DefaultCommands.Add("");
	DefaultCommands.Add("[Stat]");
	DefaultCommands.Add("Stat AI= ");
	DefaultCommands.Add("Stat Collision= ");
	DefaultCommands.Add("Stat FPS= ");
	DefaultCommands.Add("Stat Engine= ");
	DefaultCommands.Add("Stat NET= ");
	DefaultCommands.Add("Stat Geometry= ");
	DefaultCommands.Add("Stat CPULoad= ");
	DefaultCommands.Add("Stat Game= ");
	DefaultCommands.Add("Stat GPU= ");
	DefaultCommands.Add("Stat SceneRendering= ");
	DefaultCommands.Add("");
	DefaultCommands.Add("[T]");
	DefaultCommands.Add("t.MaxFPS= ");
	DefaultCommands.Add("t.OverrideFPS= ");
	DefaultCommands.Add("t.UnsteadyFPS= ");
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	const FString BaseDirectory = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir());
	const FString ConfigDirectory = FPaths::Combine(BaseDirectory, TEXT("Config"));
	const FString ConfigFilePath = FPaths::Combine(ConfigDirectory, TEXT("Commands.ini"));

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!FPaths::DirectoryExists(ConfigDirectory))
	{
		if (PlatformFile.CreateDirectory(*ConfigDirectory))
		{
			FFileHelper::SaveStringArrayToFile(DefaultCommands, *ConfigFilePath);
		}
	}
	else
	{
		if (!PlatformFile.FileExists(*ConfigFilePath))
		{
			FFileHelper::SaveStringArrayToFile(DefaultCommands, *ConfigFilePath);
		}
	}
}

void FVRDebugPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVRDebugPluginModule, VRDebugPlugin)