// Copyright 2021 DownToCode. All Rights Reserved.

#include "VRDebugWidgetFL.h"
#include "VRDebugLog.h"

void UVRDebugWidgetFL::SetButtonColour(UButton* Button, FLinearColor NormalColour, FLinearColor PressedColour, FLinearColor HoverColour)
{
	if (!Button) 
	{
		UE_LOG(VRDebugLog, Warning, TEXT("Button invalid reference in SetButtonColour."));
		return;
	}

	FButtonStyle Style = Button->WidgetStyle;

	FSlateBrush Normal = Style.Normal;
	Normal.TintColor = NormalColour;

	FSlateBrush Pressed = Style.Pressed;
	Pressed.TintColor = PressedColour;

	FSlateBrush Hover = Style.Hovered;
	Hover.TintColor = HoverColour;

	Style.SetNormal(Normal);
	Style.SetPressed(Pressed);
	Style.SetHovered(Hover);

	Button->SetStyle(Style);
}

void UVRDebugWidgetFL::SetTextboxFontSize(UEditableTextBox* EditableTextbox, int32 FontSize)
{
	FEditableTextBoxStyle Style = EditableTextbox->WidgetStyle;
	Style.Font.Size = FontSize;
	EditableTextbox->WidgetStyle = Style;
}

void UVRDebugWidgetFL::SetTextboxFontType(UEditableTextBox* EditableTextbox, EFontTypeFace Typeface)
{
	if (EditableTextbox)
	{
		FEditableTextBoxStyle Style = EditableTextbox->WidgetStyle;
		switch (Typeface)
		{
		case EFontTypeFace::Bold:
			Style.Font.TypefaceFontName = TEXT("Bold");
			break;
		case EFontTypeFace::Italic:
			Style.Font.TypefaceFontName = TEXT("Italic");
			break;
		case EFontTypeFace::Light:
			Style.Font.TypefaceFontName = TEXT("Light");
			break;
		case EFontTypeFace::Regular:
			Style.Font.TypefaceFontName = TEXT("Regular");
			break;
		default:
			return;
			break;
		}
		EditableTextbox->WidgetStyle = Style;
	}
}

void UVRDebugWidgetFL::SetTextblockFontSize(UTextBlock* Text, int32 FontSize)
{
	if (Text)
	{
		FSlateFontInfo Font = Text->Font;
		Font.Size = FontSize;
		Text->SetFont(Font);
	}
}

void UVRDebugWidgetFL::SetTextblockFontType(UTextBlock* Text, EFontTypeFace Typeface)
{
	if (Text)
	{
		FSlateFontInfo Font = Text->Font;
		switch (Typeface)
		{
		case EFontTypeFace::Bold:
			Font.TypefaceFontName = TEXT("Bold");
			break;
		case EFontTypeFace::Italic: 
			Font.TypefaceFontName = TEXT("Italic");
			break;
		case EFontTypeFace::Light:
			Font.TypefaceFontName = TEXT("Light");
			break;
		case EFontTypeFace::Regular:
			Font.TypefaceFontName = TEXT("Regular");
			break;
		default:
			return;
			break;	
		}
		Text->SetFont(Font);
	}
}

void UVRDebugWidgetFL::SetTextblockColour(UTextBlock* Text, FLinearColor Colour)
{
	FSlateColor Color(Colour);
	Text->SetColorAndOpacity(Color);
}

void UVRDebugWidgetFL::GetChildrenOfClass(UPanelWidget* ParentWidget, TSubclassOf<UWidget> WidgetComponentClass, TArray<UWidget*>& OutComponents)
{
	int32 ChildrenCount = ParentWidget->GetChildrenCount();
	for (int32 i = 0; i < ChildrenCount; i++)
	{
		if (ParentWidget->GetChildAt(i)->IsA(WidgetComponentClass))
		{
			OutComponents.Add(ParentWidget->GetChildAt(i));
		}

		if (ParentWidget->GetChildAt(i)->IsA<UPanelWidget>())
		{
			UPanelWidget* ChildPanel = Cast<UPanelWidget>(ParentWidget->GetChildAt(i));
			if (ChildPanel->HasAnyChildren())
			{
				GetChildrenOfClass(ChildPanel, WidgetComponentClass, OutComponents);
			}
		}
	}
}