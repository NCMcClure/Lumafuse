// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "VRDebugWidgetFL.generated.h"

UENUM(BlueprintType)
enum class EFontTypeFace : uint8
{
	Regular,
	Light,
	Bold,
	Italic
};

UCLASS()
class VRDEBUGPLUGIN_API UVRDebugWidgetFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Widget Blueprint Function Library")
	static void SetButtonColour(UButton* Button, FLinearColor NormalColour, FLinearColor PressedColour, FLinearColor HoverColour);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Widget Blueprint Function Library")
	static void SetTextboxFontSize(UEditableTextBox* EditableTextbox, int32 FontSize);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Widget Blueprint Function Library")
	static void SetTextboxFontType(UEditableTextBox* EditableTextbox, EFontTypeFace Typeface);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Widget Blueprint Function Library")
	static void SetTextblockFontSize(UTextBlock* Text, int32 FontSize);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Widget Blueprint Function Library")
	static void SetTextblockFontType(UTextBlock* Text, EFontTypeFace Typeface);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Widget Blueprint Function Library")
	static void SetTextblockColour(UTextBlock* Text, FLinearColor Colour);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Widget Blueprint Function Library", meta = (DeterminesOutputType = "WidgetComponentClass", DynamicOutputParam = "OutComponents"))
	static void GetChildrenOfClass(UPanelWidget* ParentWidget, TSubclassOf<UWidget> WidgetComponentClass, TArray<UWidget*>& OutComponents);
};
