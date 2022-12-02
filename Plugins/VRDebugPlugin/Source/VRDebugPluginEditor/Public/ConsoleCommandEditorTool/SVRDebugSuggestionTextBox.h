// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Slate/Public/Widgets/Input/SSuggestionTextBox.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

DECLARE_DELEGATE(FOnTextRestored);

/**
 * Implements an editable text box that can show auto-complete histories and suggestions lists.
 */
class VRDEBUGPLUGINEDITOR_API SVRDebugSuggestionTextBox : public SCompoundWidget
{

public:

	SLATE_BEGIN_ARGS(SVRDebugSuggestionTextBox)
		: _BackgroundColor(FLinearColor::White)
		, _DeleteRowAction()
		, _BackgroundImage(FCoreStyle::Get().GetBrush("SuggestionTextBox.Background"))
		, _ClearKeyboardFocusOnCommit(true)
		, _ErrorReporting()
		, _Font(FCoreStyle::Get().GetFontStyle(TEXT("NormalFont")))
		, _ForegroundColor(FCoreStyle::Get().GetSlateColor("InvertedForeground"))
		, _HintText()
		, _IsCaretMovedWhenGainFocus(true)
		, _MinDesiredWidth(0.0f)
		, _AllowContextMenu(true)
		, _RevertTextOnEscape(false)
		, _SelectAllTextOnCommit(false)
		, _SelectAllTextWhenFocused(false)
		, _TextStyle(&FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox"))
		, _SuggestionTextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("SuggestionTextBox.Text"))
		, _SuggestionListMaxHeight(250.0)
		, _Text()
	{ }

	/** The color of the background/border around the editable text. */
	SLATE_ATTRIBUTE(FSlateColor, BackgroundColor)

	SLATE_ARGUMENT(FUIAction, DeleteRowAction)

	/** The image of the background/border around the editable text. */
	SLATE_ARGUMENT(const FSlateBrush*, BackgroundImage)

	/** Whether to clear keyboard focus when pressing enter to commit changes. */
	SLATE_ATTRIBUTE(bool, ClearKeyboardFocusOnCommit)

	/** Provides an alternative mechanism for error reporting. */
	SLATE_ARGUMENT(TSharedPtr<class IErrorReportingWidget>, ErrorReporting)

	/** Font color and opacity. */
	SLATE_ATTRIBUTE(FSlateFontInfo, Font)

	/** Text color and opacity. */
	SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)

	/** Hint text that appears when there is no text in the text box. */
	SLATE_ATTRIBUTE(FText, HintText)

	/** Workaround as we loose focus when the auto completion closes. */
	SLATE_ATTRIBUTE(bool, IsCaretMovedWhenGainFocus)

	/** Minimum width that a text block should be. */
	SLATE_ATTRIBUTE(float, MinDesiredWidth)

	/** Whether the context menu can be opened */
	SLATE_ATTRIBUTE(bool, AllowContextMenu)
	
	/**Is the suggestion box used to represent and newly added commands*/
	SLATE_ATTRIBUTE(bool, IsNewCommand)

	/**Called once the text has been restored*/
	SLATE_EVENT(FOnTextRestored, OnTextRestored)

	/** Called before the history list is shown. */
	SLATE_EVENT(FOnShowingHistory, OnShowingHistory)

	/** Called before the suggestion list is shown. */
	SLATE_EVENT(FOnShowingSuggestions, OnShowingSuggestions)

	/** Called whenever the text is changed interactively by the user. */
	SLATE_EVENT(FOnTextChanged, OnTextChanged)

	/** Called when the text has been committed. */
	SLATE_EVENT(FOnTextCommitted, OnTextCommitted)

	/** Delegate to call before a context menu is opened. User returns the menu content or null to the disable context menu */
	SLATE_EVENT(FOnContextMenuOpening, OnContextMenuOpening)

	/** Whether to allow the user to back out of changes when they press the escape key. */
	SLATE_ATTRIBUTE(bool, RevertTextOnEscape)

	/** Whether to select all text when pressing enter to commit changes. */
	SLATE_ATTRIBUTE(bool, SelectAllTextOnCommit)

	/** Whether to select all text when the user clicks to give focus on the widget. */
	SLATE_ATTRIBUTE(bool, SelectAllTextWhenFocused)

	/** The styling of the text box. */
	SLATE_STYLE_ARGUMENT(FEditableTextBoxStyle, TextStyle)

	/** The styling of the suggestion text. */
	SLATE_STYLE_ARGUMENT(FTextBlockStyle, SuggestionTextStyle)

	/** The maximum height of the suggestion list. */
	SLATE_ATTRIBUTE(float, SuggestionListMaxHeight)

	/** Sets the text content for this editable text box widget. */
	SLATE_ATTRIBUTE(FText, Text)

	SLATE_END_ARGS()

public:

	/** Default constructor. */
	SVRDebugSuggestionTextBox();

public:

	/**
	 * Construct this widget.  Called by the SNew() Slate macro.
	 *
	 * @param InArgs Declaration used by the SNew() macro to construct this widget.
	 */
	void Construct(const FArguments& InArgs);

	void ConstructContextMenu(FUIAction DeleteRowAction);

	/**
	 * Returns the text string.
	 *
	 * @return The text string.
	 */
	FText GetText() const
	{
		return TextBox->GetText();
	}

	/**
	 * Sets an optional error string.
	 *
	 * If InError is a non-empty string the TextBox will the ErrorReporting provided during construction.
	 * If no error reporting was provided, the TextBox will create a default error reporter.
	 *
	 * @param InError The error string to set.
	 */
	void SetError(const FString& InError)
	{
		TextBox->SetError(InError);
	}

	/**
	 * Sets the text string currently being edited
	 *
	 * @param InNewText The new text string.
	 */
	void SetText(const TAttribute< FText >& InNewText);

	/** See the AllowContextMenu attribute */
	void SetAllowContextMenu(TAttribute< bool > InAllowContextMenu);

protected:

	/** Clears the list of suggestions and hides the suggestions list. */
	void ClearSuggestions();

	/** Sets the keyboard focus to the text box. */
	void FocusTextBox();

	/**
	 * Gets the string value of the currently selected suggestion.
	 *
	 * @return Suggestion string, or empty if no suggestion is selected.
	 */
	FString GetSelectedSuggestionString() const;

	/** Highlights the selected suggestion in the suggestion list. */
	void MarkActiveSuggestion();

	/**
	 * Sets the list of suggestions.
	 *
	 * @param SuggestionStrings The suggestion strings.
	 * @param InHistoryMode Whether the suggestions represent the input history.
	 */
	void SetSuggestions(TArray<FString>& SuggestionStrings, bool InHistoryMode);

protected:

	// SWidget overrides

	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
	virtual bool SupportsKeyboardFocus() const override;

private:

	TSharedPtr<SWidget> GetContextMenu();

	// Callback for generating a row widget for the suggestion list view.
	TSharedRef<ITableRow> HandleSuggestionListViewGenerateRow(TSharedPtr<FString> Message, const TSharedRef<STableViewBase>& OwnerTable);

	// Callback for changing the selected suggestion.
	void HandleSuggestionListViewSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo);

	// Callback for getting the highlight string in a suggestion list row widget.
	FText HandleSuggestionListWidgetHighlightText() const;

	// Callback for changing the text box input.
	void HandleTextBoxTextChanged(const FText& InText);

	// Callback committing the text box input.
	void HandleTextBoxTextCommitted(const FText& InText, ETextCommit::Type CommitInfo);

private:

	// Holds a flag to prevent recursive calls in UI callbacks.
	bool IgnoreUIUpdate;

	bool IsNewCommand;

	// Holds the box for history and auto-complete suggestions.
	TSharedPtr<SMenuAnchor> MenuAnchor;

	// Holds the current list of suggestions.
	TArray<TSharedPtr<FString> > Suggestions;

	// Holds the list view that displays the suggestions.
	TSharedPtr<SListView<TSharedPtr<FString> > > SuggestionListView;

	// Holds the index of the selected suggestion (-1 for no selection).
	int32 SelectedSuggestion;

	// Holds the editable text box.
	TSharedPtr<SEditableTextBox> TextBox;

	// The styling of the suggestion text.
	const FTextBlockStyle* SuggestionTextStyle;

	FText InitialText;

	FMenuBuilder ContextMenu;

private:

	// Holds a delegate that is executed before the input history is shown.
	FOnShowingHistory OnShowingHistory;

	// Holds a delegate that is executed before the suggestions list is shown.
	FOnShowingSuggestions OnShowingSuggestions;

	// Holds a delegate that is executed when the text has changed.
	FOnTextChanged OnTextChanged;

	// Holds a delegate that is executed when the text has been committed.
	FOnTextCommitted OnTextCommitted;
	
	//Holds a delegate that is executed when the text gets restored.
	FOnTextRestored OnTextRestored;
};
