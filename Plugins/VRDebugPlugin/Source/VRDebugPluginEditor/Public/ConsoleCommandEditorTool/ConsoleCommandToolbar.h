// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "VRDebugPluginStyle.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Layout/SSeparator.h"
#include "Runtime/Slate/Public/Widgets/Layout/SGridPanel.h"
#include "Widgets/SBoxPanel.h"

struct FToolbar
{
	FToolbar(
		FOnClicked AddCategoryClick,
		FOnClicked AddNewCommandClick,
		FOnClicked DeleteCategory, 
		FOnClicked SaveCommands, 
		FOnClicked OpenCommandsFile, 
		FOnClicked OpenCommandFileFolder)
	{
		const FText AddCategoryTxt = FText::FromString("Add Category");
		const FText AddCategoryHintTxt = FText::FromString("Category Name");
		const FText AddCategoryButtonTxt = FText::FromString("ADD");
		const FText AddCommandTxt = FText::FromString("Add new command");
		const FText AddCommandButtonTxt = FText::FromString("ADD");
		const FText RemoveCommandTxt = FText::FromString("Delete Category");
		const FText DeleteCommandButtonTxt = FText::FromString("DELETE");
		const FText SaveCommandText = FText::FromString("Save Commnads To File");
		const FText SaveCommandButtonText = FText::FromString("SAVE COMMANDS");
		const FText OpenCommandFileTxt = FText::FromString("OPEN COMMAND FILE");
		const FText OpenCommandFolderTxt = FText::FromString("OPEN FILE FOLDER");

		FTextBlockStyle TextStyleTemp;
		FSlateFontInfo Font = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"), 14);
		TextStyleTemp.SetFont(Font);
		TextStyleTemp.SetColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 1.0f)));
		const FTextBlockStyle* TextStyle = new FTextBlockStyle(TextStyleTemp);


		SAssignNew(VerticalBox, SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SGridPanel)
			+SGridPanel::Slot(0, 0) //Row 1
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(0, 5.0f, 0, 0)
			[
				SNew(STextBlock)
				.Text(AddCategoryTxt)
				.Justification(ETextJustify::Left)
				.Margin(FMargin(10.0f, 0, 0, 0))
				.Font(Font)
			]
			+SGridPanel::Slot(1, 0)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(10.0f, 5.0f, 0, 0)
			[
				SAssignNew(NewCategoryTextBox, SEditableTextBox)
				.Font(Font)
				.HintText(AddCategoryHintTxt)
				.MinDesiredWidth(200.0f)
			]
			+SGridPanel::Slot(2, 0)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(10, 5.0f, 0, 0)
			[
				SNew(SButton)
				.Text(AddCategoryButtonTxt)
				.TextStyle(TextStyle)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.OnClicked(AddCategoryClick)
			]
			+ SGridPanel::Slot(0, 1) //Row 2
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(0, 5.0f, 0, 0)
			[
				SNew(STextBlock)
				.Text(AddCommandTxt)
				.Justification(ETextJustify::Left)
				.Margin(FMargin(10.0f, 0, 0, 0))
				.Font(Font)
			]
			+ SGridPanel::Slot(1, 1)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(10.0f, 5.0f, 0, 0)
			[
				SNew(SButton)
				.Text(AddCommandButtonTxt)
				.TextStyle(TextStyle)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.OnClicked(AddNewCommandClick)
			]
			+ SGridPanel::Slot(0, 2) //Row 3
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(0, 5.0f, 0, 0)
			[
				SNew(STextBlock)
				.Text(RemoveCommandTxt)
				.Margin(FMargin(10.0f, 0, 0, 0))
				.Justification(ETextJustify::Left)
				.Font(Font)
			]
			+ SGridPanel::Slot(1, 2)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(10.0f, 5.0f, 0, 0)
			[
				SNew(SButton)
				.Text(DeleteCommandButtonTxt)
				.TextStyle(TextStyle)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.OnClicked(DeleteCategory)
			]
			+ SGridPanel::Slot(0, 3) //Row 4
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(0, 5.0f, 0, 5.0f)
			[
				SNew(STextBlock)
				.Text(SaveCommandText)
				.Justification(ETextJustify::Left)
				.Margin(FMargin(10.0f, 0, 0, 0))
				.Font(Font)
			]
			+ SGridPanel::Slot(1, 3)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(10.0f, 5.0f, 0, 5.0f)
			[
				SNew(SButton)
				.Text(SaveCommandButtonText)
				.TextStyle(TextStyle)
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.OnClicked(SaveCommands)
			]
			+ SGridPanel::Slot(2, 3) 
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(10.0f, 5.0f, 0, 5.0f)
			[
				SNew(SButton)
				.Text(OpenCommandFileTxt)
				.TextStyle(TextStyle)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.OnClicked(OpenCommandsFile)
			]
			+ SGridPanel::Slot(3, 3)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(10.0f, 5.0f, 0, 5.0f)
			[
				SNew(SButton)
				.Text(OpenCommandFolderTxt)
				.TextStyle(TextStyle)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.OnClicked(OpenCommandFileFolder)
			]
			
		];
	}

	TSharedRef<SVerticalBox> GetReference()
	{
		return VerticalBox.ToSharedRef();
	}

	FString GetNewCategoryName()
	{
		return NewCategoryTextBox->GetText().ToString();
	}

	void EmptyNewCategoryTextBox()
	{
		NewCategoryTextBox->SetText(FText::FromString(""));
	}

public:
	TSharedPtr<SVerticalBox> VerticalBox;

	TSharedPtr<SEditableTextBox> NewCategoryTextBox;
};