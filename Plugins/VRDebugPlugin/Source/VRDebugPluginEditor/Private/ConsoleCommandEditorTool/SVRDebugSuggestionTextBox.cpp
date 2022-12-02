// Copyright 2019 DownToCode. All Rights Reserved.

#include "ConsoleCommandEditorTool/SVRDebugSuggestionTextBox.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Input/SMenuAnchor.h"
#include "Widgets/Views/SListView.h"


/* SVRDebugSuggestionTextBox structors
 *****************************************************************************/

SVRDebugSuggestionTextBox::SVRDebugSuggestionTextBox()
	: IgnoreUIUpdate(false)
	, IsNewCommand(false)
	, SelectedSuggestion(-1)
	, ContextMenu(FMenuBuilder(true, NULL))
{ }

/* SVRDebugSuggestionTextBox interface
 *****************************************************************************/

void SVRDebugSuggestionTextBox::Construct(const FArguments& InArgs)
{
	SuggestionTextStyle = InArgs._SuggestionTextStyle;
	InitialText = InArgs._Text.Get();
	IsNewCommand = InArgs._IsNewCommand.Get();
	ConstructContextMenu(InArgs._DeleteRowAction);

	OnShowingHistory = InArgs._OnShowingHistory;
	OnShowingSuggestions = InArgs._OnShowingSuggestions;
	OnTextChanged = InArgs._OnTextChanged;
	OnTextCommitted = InArgs._OnTextCommitted;
	OnTextRestored = InArgs._OnTextRestored;

	ChildSlot
		[
			SAssignNew(MenuAnchor, SMenuAnchor)
			.Placement(MenuPlacement_ComboBox)
		[
			SAssignNew(TextBox, SEditableTextBox)
			.BackgroundColor(InArgs._BackgroundColor)
			.ClearKeyboardFocusOnCommit(InArgs._ClearKeyboardFocusOnCommit.Get())
			.ErrorReporting(InArgs._ErrorReporting)
			.Font(InArgs._Font)
			.ForegroundColor(InArgs._ForegroundColor)
			.HintText(InArgs._HintText)
			.IsCaretMovedWhenGainFocus(InArgs._IsCaretMovedWhenGainFocus)
			.MinDesiredWidth(InArgs._MinDesiredWidth)
			.AllowContextMenu(true)
			.OnContextMenuOpening(FOnContextMenuOpening::CreateRaw(this, &SVRDebugSuggestionTextBox::GetContextMenu))
			.RevertTextOnEscape(InArgs._RevertTextOnEscape.Get())
			.SelectAllTextOnCommit(InArgs._SelectAllTextOnCommit)
			.SelectAllTextWhenFocused(InArgs._SelectAllTextWhenFocused.Get())
			.Style(InArgs._TextStyle)
			.Text(InArgs._Text)
			.OnTextChanged(this, &SVRDebugSuggestionTextBox::HandleTextBoxTextChanged)
			.OnTextCommitted(this, &SVRDebugSuggestionTextBox::HandleTextBoxTextCommitted)
		]
		.MenuContent
		(
			SNew(SBorder)
			.BorderImage(InArgs._BackgroundImage)
			.Padding(FMargin(2))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.MaxHeight(InArgs._SuggestionListMaxHeight)
				[
					SAssignNew(SuggestionListView, SListView< TSharedPtr<FString> >)
					.ItemHeight(18)
					.ListItemsSource(&Suggestions)
					.SelectionMode(ESelectionMode::Single)
					.OnGenerateRow(this, &SVRDebugSuggestionTextBox::HandleSuggestionListViewGenerateRow)
					.OnSelectionChanged(this, &SVRDebugSuggestionTextBox::HandleSuggestionListViewSelectionChanged)
				]
			]
		)
	];
}

void SVRDebugSuggestionTextBox::ConstructContextMenu(FUIAction DeleteRowAction)
{
	FUIAction Action_RestoreRow(FExecuteAction::CreateLambda([&]() {
		SetText(InitialText);
		TextBox->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
		OnTextRestored.ExecuteIfBound();
	}));

	ContextMenu.BeginSection(NAME_None, FText::FromString("Command Operations"));
	ContextMenu.AddMenuEntry(
		FText::FromString("Delete Row"),
		FText::FromString("Deletes the row on which the context menu was opened"),
		FSlateIcon(),
		DeleteRowAction,
		NAME_None,
		EUserInterfaceActionType::Button);
	ContextMenu.AddMenuEntry(
		FText::FromString("Restore Row"),
		FText::FromString("Restores this row to what it was"),
		FSlateIcon(),
		Action_RestoreRow,
		NAME_None,
		EUserInterfaceActionType::Button);

	ContextMenu.EndSection();
}

void SVRDebugSuggestionTextBox::SetText(const TAttribute<FText>& InNewText)
{
	IgnoreUIUpdate = true;

	TextBox->SetText(InNewText);

	IgnoreUIUpdate = false;
}

TSharedPtr<SWidget> SVRDebugSuggestionTextBox::GetContextMenu()
{
	return ContextMenu.MakeWidget();
}

/* SVRDebugSuggestionTextBox implementation
 *****************************************************************************/

void SVRDebugSuggestionTextBox::ClearSuggestions()
{
	SelectedSuggestion = -1;

	MenuAnchor->SetIsOpen(false);
	Suggestions.Empty();
}

void SVRDebugSuggestionTextBox::FocusTextBox()
{
	FWidgetPath TextBoxPath;

	FSlateApplication::Get().GeneratePathToWidgetUnchecked(TextBox.ToSharedRef(), TextBoxPath);
	FSlateApplication::Get().SetKeyboardFocus(TextBoxPath, EFocusCause::SetDirectly);
}

FString SVRDebugSuggestionTextBox::GetSelectedSuggestionString() const
{
	FString SuggestionString = *Suggestions[SelectedSuggestion];

	return SuggestionString.Replace(TEXT("\t"), TEXT(""));
}

void SVRDebugSuggestionTextBox::MarkActiveSuggestion()
{
	IgnoreUIUpdate = true;

	if (SelectedSuggestion >= 0)
	{
		TSharedPtr<FString> Suggestion = Suggestions[SelectedSuggestion];

		SuggestionListView->SetSelection(Suggestion);

		if (!SuggestionListView->IsItemVisible(Suggestion))
		{
			SuggestionListView->RequestScrollIntoView(Suggestion);
		}

		TextBox->SetText(FText::FromString(GetSelectedSuggestionString()));
	}
	else
	{
		SuggestionListView->ClearSelection();
	}

	IgnoreUIUpdate = false;
}

void SVRDebugSuggestionTextBox::SetSuggestions(TArray<FString>& SuggestionStrings, bool InHistoryMode)
{
	FString SelectionText;

	// cache previously selected suggestion
	if ((SelectedSuggestion >= 0) && (SelectedSuggestion < Suggestions.Num()))
	{
		SelectionText = *Suggestions[SelectedSuggestion];
	}

	SelectedSuggestion = -1;

	// refill suggestions
	Suggestions.Empty();

	for (int32 i = 0; i < SuggestionStrings.Num(); ++i)
	{
		Suggestions.Add(MakeShareable(new FString(SuggestionStrings[i])));

		if (SuggestionStrings[i] == SelectionText)
		{
			SelectedSuggestion = i;
		}
	}

	if (Suggestions.Num())
	{
		// @todo Slate: make the window title not flicker when the box toggles visibility
		MenuAnchor->SetIsOpen(true, false);
		SuggestionListView->RequestScrollIntoView(Suggestions.Last());

		FocusTextBox();
	}
	else
	{
		MenuAnchor->SetIsOpen(false);
	}
}
/* SWidget overrides
 *****************************************************************************/

void SVRDebugSuggestionTextBox::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	//	MenuAnchor->SetIsOpen(false);
}

FReply SVRDebugSuggestionTextBox::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	const FString& InputTextStr = TextBox->GetText().ToString();

	if (MenuAnchor->IsOpen())
	{
		if (KeyEvent.GetKey() == EKeys::Up)
		{
			// backward navigate the list of suggestions
			if (SelectedSuggestion < 0)
			{
				SelectedSuggestion = Suggestions.Num() - 1;
			}
			else
			{
				--SelectedSuggestion;
			}

			MarkActiveSuggestion();

			return FReply::Handled();
		}
		else if (KeyEvent.GetKey() == EKeys::Down)
		{
			// forward navigate the list of suggestions
			if (SelectedSuggestion < Suggestions.Num() - 1)
			{
				++SelectedSuggestion;
			}
			else
			{
				SelectedSuggestion = -1;
			}

			MarkActiveSuggestion();

			return FReply::Handled();
		}
		else if (KeyEvent.GetKey() == EKeys::Tab)
		{
			// auto-complete the highlighted suggestion
			if (Suggestions.Num())
			{
				if ((SelectedSuggestion >= 0) && (SelectedSuggestion < Suggestions.Num()))
				{
					MarkActiveSuggestion();
					HandleTextBoxTextCommitted(TextBox->GetText(), ETextCommit::OnEnter);
				}
				else
				{
					SelectedSuggestion = 0;

					MarkActiveSuggestion();
				}
			}

			return FReply::Handled();
		}
	}
	else
	{
		if ((KeyEvent.GetKey() == EKeys::Up) || (KeyEvent.GetKey() == EKeys::Down))
		{
			// show the input history
			if (OnShowingHistory.IsBound())
			{
				TArray<FString> HistoryStrings;

				OnShowingHistory.ExecuteIfBound(HistoryStrings);

				if (HistoryStrings.Num() > 0)
				{
					SetSuggestions(HistoryStrings, true);

					if (KeyEvent.GetKey() == EKeys::Up)
					{
						SelectedSuggestion = HistoryStrings.Num() - 1;
					}
					else
					{
						SelectedSuggestion = 0;
					}

					MarkActiveSuggestion();
				}
			}

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

bool SVRDebugSuggestionTextBox::SupportsKeyboardFocus() const
{
	return true;
}

void SVRDebugSuggestionTextBox::SetAllowContextMenu(TAttribute<bool> InAllowContextMenu)
{
	TextBox->SetAllowContextMenu(InAllowContextMenu);
}

/* SVRDebugSuggestionTextBox callbacks
 *****************************************************************************/

TSharedRef<ITableRow> SVRDebugSuggestionTextBox::HandleSuggestionListViewGenerateRow(TSharedPtr<FString> Text, const TSharedRef<STableViewBase>& OwnerTable)
{
	FString Left, Right, Combined;

	if (Text->Split(TEXT("\t"), &Left, &Right))
	{
		Combined = Left + Right;
	}
	else
	{
		Combined = *Text;
	}

	return SNew(STableRow<TSharedPtr<FString> >, OwnerTable)
		[
			SNew(SBox)
			[
				SNew(STextBlock)
				.HighlightText(this, &SVRDebugSuggestionTextBox::HandleSuggestionListWidgetHighlightText)
		.TextStyle(SuggestionTextStyle)
		.Text(FText::FromString(Combined))
			]
		];
}

void SVRDebugSuggestionTextBox::HandleSuggestionListViewSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo)
{
	if (!IgnoreUIUpdate)
	{
		for (int32 i = 0; i < Suggestions.Num(); ++i)
		{
			if (NewValue == Suggestions[i])
			{
				SelectedSuggestion = i;

				MarkActiveSuggestion();
				FocusTextBox();

				break;
			}
		}
	}
}

FText SVRDebugSuggestionTextBox::HandleSuggestionListWidgetHighlightText() const
{
	return TextBox->GetText();
}

void SVRDebugSuggestionTextBox::HandleTextBoxTextChanged(const FText& InText)
{
	if (!IgnoreUIUpdate)
	{
		if (!IsNewCommand)
		{
			if (!InText.EqualTo(InitialText))
			{
				TextBox->SetColorAndOpacity(FLinearColor(1, 0, 0, 1));
			}
			else
			{
				TextBox->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
			}
		}

		const FString& InputTextStr = TextBox->GetText().ToString();

		if (!InputTextStr.IsEmpty() && OnShowingSuggestions.IsBound())
		{
			TArray<FString> SuggestionStrings;

			OnShowingSuggestions.ExecuteIfBound(InText.ToString(), SuggestionStrings);

			for (int32 Index = 0; Index < SuggestionStrings.Num(); ++Index)
			{
				FString& StringRef = SuggestionStrings[Index];

				StringRef = StringRef.Left(InputTextStr.Len()) + TEXT("\t") + StringRef.RightChop(InputTextStr.Len());
			}

			SetSuggestions(SuggestionStrings, false);
		}
		else
		{
			ClearSuggestions();
		}
	}

	OnTextChanged.ExecuteIfBound(InText);
}

void SVRDebugSuggestionTextBox::HandleTextBoxTextCommitted(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (!MenuAnchor->IsOpen())
	{
		OnTextCommitted.ExecuteIfBound(InText, CommitInfo);
	}

	if ((CommitInfo == ETextCommit::OnEnter) || (CommitInfo == ETextCommit::OnCleared))
	{
		ClearSuggestions();
	}
}
