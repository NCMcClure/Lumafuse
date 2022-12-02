// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"
#include "Widgets/Input/SEditableText.h"
#include "TreeItem.h"

typedef STreeView<FTreeItemPtr> STreePtr;

class STree : public SCompoundWidget
{

public:

	SLATE_BEGIN_ARGS(STree){}

	SLATE_END_ARGS()

public:
	/** Widget constructor */
	void Construct(const FArguments& Args);

	/** Destructor */
	~STree();

	/** @return Returns the currently selected category item */
	FTreeItemPtr GetSelectedItem() const;

	/** Selects the specified category */
	void SelectItem(const FTreeItemPtr& CategoryToSelect);

	/** @return Returns true if the specified item is currently expanded in the tree */
	bool IsItemExpanded(const FTreeItemPtr Item) const;

	/** Rebuilds the category tree from scratch */
	void RebuildTree();

	/**ConsoleCommandTab button clicks*/
	void AddNewCommand(const FText& InText);
	void AddNewCategory(const FText& InText);
	void DeleteCategory(const FTreeItemPtr Item = nullptr);

	/**Go through the current categories and commands in the widget and return their value in a map*/
	TMap<FString, TArray<FString>> GetEditedConsoleCommands();
private:

	/** Called to generate a widget for the specified tree item */
	TSharedRef<ITableRow> Tree_OnGenerateRow(FTreeItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);

	/** Given a tree item, fills an array of child items */
	void Tree_OnGetChildren(FTreeItemPtr Item, TArray< FTreeItemPtr >& OutChildren);

	/** Called when the user clicks on an  item, or when selection changes by some other means */
	void Tree_OnSelectionChanged(FTreeItemPtr Item, ESelectInfo::Type SelectInfo);

private:
	/** The tree view widget*/
	TSharedPtr<STreePtr> TreeView;

	TArray<FString> EditorConsoleCommands;

	/** The Core Data for the Tree Viewer! */
	TMap<FString, TArray<FString>> CategoryCommandsList;
	TArray<FTreeItemPtr> Categories;


private:
	//Suggestion textbox
	void OnShowingSuggestion(const FString& NewText, TArray<FString>& OutSuggestions);
	void DeleteRow(FTreeItemPtr Item);
	void OnCommandTextChanged(const FText& InText, FTreeItemPtr Item);

	/**Drag and drop operations*/
	TOptional<EItemDropZone> OnCanAcceptDrop(const FDragDropEvent& InDragDropEvent, EItemDropZone InDropZone, FTreeItemPtr Item);
	FReply DragDetected(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FText ItemText);
	FReply OnAcceptDrop(const FDragDropEvent& InDragDropEvent, EItemDropZone InDropZone, FTreeItemPtr Item);
	void DropCategory(FTreeItemPtr ItemDropped, FTreeItemPtr Item, EItemDropZone ItemDropZone);
	void DropCommand(FTreeItemPtr ItemDropped, FTreeItemPtr Item, EItemDropZone ItemDropZone);
	/**End Drag and drop operations*/
};