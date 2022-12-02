// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 *
 */
class FVRDebugPluginStyle
{
public:
	static void Initialize();

	static void Shutdown();

	static void ReloadTextures();

	static const ISlateStyle& Get();

	static FName GetStyleSetName();

	static const FSlateBrush* GetBrush(FString Name);

	static const FSlateBrush GetBrushRef(FString Name);

private:

	static TSharedRef<class FSlateStyleSet> Create();

private:

	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};
