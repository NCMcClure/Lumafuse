// Copyright 2021 DownToCode. All Rights Reserved.

#include "VRDebugFL.h"
#include "VRDebugLog.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"
#include "Runtime/Engine/Classes/Engine/AssetManager.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"

void UVRDebugFL::RunConsoleCommand(AActor* AnyActor, const FString CommandToRun)
{
	if (AnyActor->IsValidLowLevel())
	{
		UWorld* World = AnyActor->GetWorld();
		if (!World)
		{
			UE_LOG(VRDebugLog, Warning, TEXT("AnyActor world isn't valid inside RunConsoleCommand function from VRDebugFL."));
			return;
		}

		World->GetFirstPlayerController()->ConsoleCommand(CommandToRun, true);
	}
	else
	{
		UE_LOG(VRDebugLog, Warning, TEXT("AnyActor reference isn't valid inside RunConsoleCommand function from VRDebugFL."));
		return;
	}
}

void UVRDebugFL::SaveCaptureComponent2D(UObject* WorldContextObject, USceneCaptureComponent2D* SceneCaptureComponent2D, FString ImagePath, FLatentActionInfo LatentInfo)
{
	//Register call to latent function
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		if (LatentActionManager.FindExistingAction<FSaveCaptureComponent2D>(LatentInfo.CallbackTarget, LatentInfo.UUID) == NULL)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FSaveCaptureComponent2D(SceneCaptureComponent2D, ImagePath, LatentInfo));
		}
	}
}

FString UVRDebugFL::GetAndroidProjectPath()
{
#if PLATFORM_ANDROID
	extern FString GFilePathBase;
	return GFilePathBase + FString("/UE4Game/") + FApp::GetProjectName() + FString("/");
#else
	return FString("");
#endif
}

void UVRDebugFL::TimestampFile(FString FilePath)
{
	FDateTime Now = FDateTime::Now();
	FString Year = FString::FromInt(Now.GetYear());
	FString Month = FString::FromInt(Now.GetMonth());
	FString Day = FString::FromInt(Now.GetDay());
	FString Hour = FString::FromInt(Now.GetHour());
	FString Minute = FString::FromInt(Now.GetMinute());
	FString Second = FString::FromInt(Now.GetSecond());
	int32 Index;
	FilePath.FindLastChar('.', Index);
	if (Index == -1) return;

	FString FilePathNoExtension = FilePath.Left(Index);
	FString NewFilePath = FilePathNoExtension + Year + "-" + Month + "-" + Day + "-" + Hour + "-" + Minute + "-" + Second + ".png";

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		UE_LOG(VRDebugLog, Warning, TEXT("Could Not Find File"));
		return;
	}

	if (!FPlatformFileManager::Get().GetPlatformFile().MoveFile(*NewFilePath, *FilePath))
	{
		UE_LOG(VRDebugLog, Warning, TEXT("Could not move file"));
	}
}

bool UVRDebugFL::CreateFolder(const FString FolderPath)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*FolderPath)) 
	{
		PlatformFile.CreateDirectory(*FolderPath);
		if (!PlatformFile.DirectoryExists(*FolderPath)) return false;
	}
	return true;
}

TArray<FName> UVRDebugFL::GetAllLevels()
{
	TArray<FName> Levels;
	TArray<FAssetData> Assets = GetAllLevelsAssets();
	for (const FAssetData Asset : Assets)
	{
		Levels.Add(Asset.AssetName);
	}
	return Levels;
}

TArray<FAssetData> UVRDebugFL::GetAllLevelsAssets()
{
	TArray<FAssetData> Assets;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegestry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassNames.Add(UWorld::StaticClass()->GetFName());
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add("/Game");

	AssetRegestry.GetAssets(Filter, Assets);
	return Assets;
}

TArray<FName> UVRDebugFL::GetAllSubLevelsOfLevel(FName LevelName)
{
	TArray<FName> SubLevels;
	TArray<FAssetData> Assets = GetAllLevelsAssets();
	UWorld* World = nullptr;
	for (const FAssetData Asset : Assets)
	{
		if (Asset.AssetName == LevelName)
		{
			World = Cast<UWorld>(Asset.GetAsset());
		}
	}

	if (World)
	{
		const TArray<ULevelStreaming*>& LevelStreaming = World->GetStreamingLevels();
		for (int32 i = 0; i < LevelStreaming.Num(); i++)
		{
			if (LevelStreaming[i]->GetWorld())
			{
				SubLevels.Add(FPackageName::GetShortFName(*LevelStreaming[i]->GetWorldAssetPackageFName().ToString()));
			}
		}
	}

	return SubLevels;
}

ETraceTypeQuery UVRDebugFL::ConvertToTraceTypeQuery(ECollisionChannel CollisionChannel)
{
	return UEngineTypes::ConvertToTraceType(CollisionChannel);
}
