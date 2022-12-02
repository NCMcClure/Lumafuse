// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveCaptureComponentLatent.h"
#include "Runtime/AssetRegistry/Public/AssetData.h"
#include "VRDebugFL.generated.h"

UCLASS()
class VRDEBUGPLUGIN_API UVRDebugFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | VR Debug FL")
	static void RunConsoleCommand(AActor* AnyActor, const FString CommandToRun);

	/**
    *Save Capture Component 2D to an image
    *@param SceneCaptureComponent2D The scene capture component 2D that will be used to take the picture in the scene
    *@param ImagePath The full image path including the file extension
    */
    UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Image Processing", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo"))
    static void SaveCaptureComponent2D(UObject* WorldContextObject, class USceneCaptureComponent2D* SceneCaptureComponent2D, FString ImagePath, struct FLatentActionInfo LatentInfo);

    /**
    *On Android the get directories blueprint functions return a relative path. This gets the absolute path.
    *@return This function returns the absolute path of the game.
    */
    UFUNCTION(BlueprintPure, Category = "VR Debug Plugin | IO")
    static FString GetAndroidProjectPath();
	
    UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | IO")
    static void TimestampFile(FString FilePath);
	
	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | IO")
	static bool CreateFolder(const FString FolderPath);

    UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | VR Debug FL")
    static TArray<FName> GetAllLevels();
    
    UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | VR Debug FL")
    static TArray<FName> GetAllSubLevelsOfLevel(FName LevelName);

	UFUNCTION(BlueprintPure, Category = "VR Debug Plugin | Collision")
	static ETraceTypeQuery ConvertToTraceTypeQuery(ECollisionChannel CollisionChannel);
private:

    static TArray<FAssetData> GetAllLevelsAssets();
};
