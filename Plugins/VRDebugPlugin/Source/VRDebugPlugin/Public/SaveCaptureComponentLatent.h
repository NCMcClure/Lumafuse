// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"
#include "Runtime/Core/Public/Async/Async.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#if PLATFORM_ANDROID
#include "Runtime/ApplicationCore/Public/Android/AndroidPlatformApplicationMisc.h"
#endif
#include "VRDebugLog.h"

class FSaveCaptureComponent2D : public FPendingLatentAction
{

public:
	/** Function to execute on completion */
	FName ExecutionFunction;
	/** Link to fire on completion */
	int32 OutputLink;
	/** Object to call callback on upon completion */
	FWeakObjectPtr CallbackTarget;
	//Future used to check if Async function result it's ready
	TFuture<bool> Result;

	FSaveCaptureComponent2D(class USceneCaptureComponent2D* InSceneCaptureComponent2D, FString InImagePath, const FLatentActionInfo& LatentInfo)
		: ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, SceneCaptureComponent2D(InSceneCaptureComponent2D)
		, ImagePath(InImagePath)
		, RunFirstTime(false)
	{

	}

	//UBlueprintAsyncActionBase interface
	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		if (!RunFirstTime)
		{
			RunFirstTime = true;

			if ((SceneCaptureComponent2D == nullptr) || (SceneCaptureComponent2D->TextureTarget == nullptr))
			{
				Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
				return;
			}

			FRenderTarget* RenderTarget = SceneCaptureComponent2D->TextureTarget->GameThread_GetRenderTargetResource();

			if (RenderTarget == nullptr)
			{
				Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
				return;
			}

			TArray<FColor> RawPixels;
			if (SceneCaptureComponent2D->TextureTarget->GetFormat() != PF_B8G8R8A8)
			{
				UE_LOG(VRDebugLog, Warning, TEXT("Tried to save texture render target with unsupported format. Only supported format is PF_B8G8R8A8"));
				Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
				return;
			}

			if (!RenderTarget->ReadPixels(RawPixels))
			{
				Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
				return;
			}

			// Convert to FColor.
			FColor ClearFColour = FColor::Black;
			TArray<FColor> BitmapColour;

			NodeDescription = "Reading pixels";

			for (auto& Pixel : RawPixels)
			{
				if (ImagePath.ToLower().EndsWith(".bmp"))
					BitmapColour.Add(Pixel);
				const uint8 PR = Pixel.R;
				const uint8 PB = Pixel.B;
				Pixel.R = PB;
				Pixel.B = PR;

				Pixel.A = ((Pixel.R == ClearFColour.R) && (Pixel.G == ClearFColour.G) && (Pixel.B == ClearFColour.B)) ? 0 : 255;
			}

			TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtension(ImagePath);

			const int32 Width = SceneCaptureComponent2D->TextureTarget->SizeX;
			const int32 Height = SceneCaptureComponent2D->TextureTarget->SizeY;

			NodeDescription = "Reversing image";

#if PLATFORM_ANDROID && LESS25
			if (UGameplayStatics::GetPlatformName() == TEXT("Android") && !FAndroidMisc::ShouldUseVulkan())
				RawPixels = ReverseImage(RawPixels, Width, Height);
#endif

			NodeDescription = "Saving file";
			if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&RawPixels[0], RawPixels.Num() * sizeof(FColor), Width, Height, ERGBFormat::RGBA, 8))
			{
				Result = Async(EAsyncExecution::Thread, [=] 
				{
					bool result;

					if (ImagePath.ToLower().EndsWith(".bmp"))
						result = FFileHelper::CreateBitmap(*ImagePath, Width, Height, BitmapColour.GetData(), nullptr, &IFileManager::Get(), nullptr, false);
					else
						result = FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(), *ImagePath);

					if (result)
						return true;
					else
						return false;
					});
			}
			else
			{
				Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
				return;
			}

		}
		else
		{
			if (Result.IsReady())
			{
				Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
			}
		}
	}
	//~UBlueprintAsyncActionBase interface

#if WITH_EDITOR
	virtual FString GetDescription() const override
	{
		return NodeDescription;
	}
#endif
private:
	TArray<FColor> ReverseImage(TArray<FColor> ImageArray, int32 Width, int32 Height)
	{
		TArray<FColor> OutArray;

		for (int32 i = Height - 1; i >= 0; i--)
		{
			for (int32 x = 0; x < Width; x++)
			{
				OutArray.Add(ImageArray[i * Width + x]);
			}
		}

		return OutArray;
	}

	//Get the image wrapper based on the extension
	TSharedPtr<IImageWrapper> GetImageWrapperByExtension(const FString InImagePath)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::GetModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		if (InImagePath.EndsWith(".png"))
		{
			return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		}
		else if (InImagePath.EndsWith(".jpg") || InImagePath.EndsWith(".jpeg"))
		{
			return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		}
		else if (InImagePath.EndsWith(".bmp"))
		{
			return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
		}
		else if (InImagePath.EndsWith(".ico"))
		{
			return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);
		}
		else if (InImagePath.EndsWith(".exr"))
		{
			return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
		}
		else if (InImagePath.EndsWith(".icns"))
		{
			return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
		}

		return nullptr;
	}

private:
	class USceneCaptureComponent2D* SceneCaptureComponent2D;
	FString ImagePath;
	bool RunFirstTime;
	FString NodeDescription;
};
