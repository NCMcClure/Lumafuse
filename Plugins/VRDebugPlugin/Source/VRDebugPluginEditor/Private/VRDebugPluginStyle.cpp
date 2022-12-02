// Copyright 2019 DownToCode. All Rights Reserved.

#include "VRDebugPluginStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FVRDebugPluginStyle::StyleInstance = NULL;

void FVRDebugPluginStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FVRDebugPluginStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FVRDebugPluginStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("FVRDebugPluginStyle"));
	return StyleSetName;
}

const FSlateBrush* FVRDebugPluginStyle::GetBrush(FString Name)
{
	return StyleInstance->GetBrush(FName(*Name));
}

const FSlateBrush FVRDebugPluginStyle::GetBrushRef(FString Name)
{
	return *StyleInstance->GetBrush(FName(*Name));
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon256x256(256.0f, 256.0f);

TSharedRef< FSlateStyleSet > FVRDebugPluginStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("FVRDebugPluginStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ToolbarButton", new IMAGE_BRUSH(TEXT("ToolbarButton"), Icon40x40));
	Style->Set("WhiteBackground", new IMAGE_BRUSH(TEXT("WhiteBackground"), Icon256x256));
	Style->Set("AddButton", new IMAGE_BRUSH(TEXT("PlusSymbol"), Icon20x20));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FVRDebugPluginStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FVRDebugPluginStyle::Get()
{
	return *StyleInstance;
}

