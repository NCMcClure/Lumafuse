// Copyright 2019 DownToCode. All Rights Reserved.

#include "MotionControllerCustomization.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"

void SMotionControllerWidget::Construct(const FArguments& InArgs, FName InHandSide)
{
	check(InArgs._OnMotionControllerChanged.IsBound());
	check(InArgs._OnGetMotionControllerText.IsBound());
	check(InArgs._OnGetMotionControllers.IsBound());

	OnMotionControllerChanged = InArgs._OnMotionControllerChanged;
	OnGetMotionControllerText = InArgs._OnGetMotionControllerText;
	OnGetMotionControllers = InArgs._OnGetMotionControllers;
	HandSide = InHandSide;

	this->ChildSlot
	[
		SNew(SComboButton)
		.ContentPadding(1)
		.OnGetMenuContent(this, &SMotionControllerWidget::BuildMotionControllerMenu)
		.ButtonContent()
		[
			SNew(SEditableTextBox)
			.RevertTextOnEscape(true)
			.SelectAllTextWhenFocused(true)
			.Text(this, &SMotionControllerWidget::GetMotionControllerText)
			.OnTextCommitted(this, &SMotionControllerWidget::OnMotionControllerValueTextCommitted)
		]
	];
}

TSharedRef<SWidget> SMotionControllerWidget::BuildMotionControllerMenu()
{
	TArray<FName> MotionController = OnGetMotionControllers.Execute();

	FMenuBuilder MenuBuilder(true, NULL);
	for (FName Name : MotionController)
	{
		FUIAction MenuAction(FExecuteAction::CreateSP(this, &SMotionControllerWidget::OnMotionControllerComboValueCommitted, Name));
		MenuBuilder.AddMenuEntry(FText::FromName(Name), FText(), FSlateIcon(), MenuAction);
	}
	return MenuBuilder.MakeWidget();
}

FText SMotionControllerWidget::GetMotionControllerText() const
{
	return OnGetMotionControllerText.Execute(HandSide);
}

void SMotionControllerWidget::OnMotionControllerValueTextCommitted(const FText& InNewText, ETextCommit::Type InTextCommit)
{
	FName NewMotionController = *InNewText.ToString();
	OnMotionControllerChanged.Execute(NewMotionController, HandSide);
}

void SMotionControllerWidget::OnMotionControllerComboValueCommitted(FName InMotionController)
{
	FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
	OnMotionControllerChanged.Execute(InMotionController, HandSide);
}
