// Copyright 2019 DownToCode. All Rights Reserved.

#include "ConsoleCommandEditorTool/CommandsTreeView.h"
#include "ConsoleCommandEditorTool/TreeItem.h"
#include "ConsoleCommandEditorTool/TreeDragDropOp.h"
#include "ConsoleCommandEditorTool/SVRDebugSuggestionTextBox.h"
#include "VRDebugPluginStyle.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"
#include "ConsoleCommandEditorTool/ConsoleCommandHelper.h"

void STree::Construct(const FArguments& Args)
{
	FConsoleCommandHelper::GetEditorConsoleCommands(EditorConsoleCommands);
	FConsoleCommandHelper::GetConsoleCommandsIniFile(CategoryCommandsList);

	//Build Core Data
	RebuildTree();

	//Build the tree view of the above core data
	TreeView = SNew(STreePtr)
		.SelectionMode(ESelectionMode::Single) // For now we only support selecting a single folder in the tree
		.ClearSelectionOnClick(false)		// Don't allow user to select nothing.
		.TreeItemsSource(&Categories)
		.OnGenerateRow(this, &STree::Tree_OnGenerateRow)
		.OnGetChildren(this, &STree::Tree_OnGetChildren)
		.OnSelectionChanged(this, &STree::Tree_OnSelectionChanged);

	ChildSlot.AttachWidget(TreeView.ToSharedRef());
}

STree::~STree()
{
}

void STree::RebuildTree()
{
	Categories.Empty();
	FConsoleCommandHelper::GetConsoleCommandsIniFile(CategoryCommandsList);
	TArray<FString> TempCategories;
	CategoryCommandsList.GetKeys(TempCategories);

	for (int32 i = TempCategories.Num() - 1; i >= 0; i--)
	{
		if (TempCategories[i] == "Custom" && !(CategoryCommandsList.Find(TempCategories[i])->Num() > 1))
			continue;
		
		TSharedRef<FTreeItem> Category = MakeShareable(new FTreeItem(NULL, TempCategories[i]));
		Categories.Add(Category);
		for (FString String : *CategoryCommandsList.Find(TempCategories[i]))
		{
			if (TempCategories[i] == "Custom" && String == "DONOTDELETE")
				continue;

			TSharedRef<FTreeItem> Command = MakeShareable(new FTreeItem(Category, String));
			Category->AddCommand(Command);
		}
	}

	//Refresh
	if (TreeView.IsValid())
	{
		TreeView->RequestTreeRefresh();
	}
}

FTreeItemPtr STree::GetSelectedItem() const
{
	if (TreeView.IsValid())
	{
		auto SelectedItems = TreeView->GetSelectedItems();
		if (SelectedItems.Num() > 0)
		{
			const auto& SelectedCategoryItem = SelectedItems[0];
			return SelectedCategoryItem;
		}
	}

	return NULL;
}

void STree::SelectItem(const FTreeItemPtr& CategoryToSelect)
{
	if (ensure(TreeView.IsValid()))
	{
		TreeView->SetSelection(CategoryToSelect);
	}
}

bool STree::IsItemExpanded(const FTreeItemPtr Item) const
{
	return TreeView->IsItemExpanded(Item);
}

TSharedRef<ITableRow> STree::Tree_OnGenerateRow(FTreeItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	const FText& Null = FText::FromString("THIS WAS NULL SOMEHOW");
	if (!Item.IsValid())
	{
		return SNew(STableRow<FTreeItemPtr>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(Null)
		];
	}
	const FText& ItemText = FText::FromString(Item->GetDisplayName());
	if (!Item->IsCategory())
	{
		FUIAction Action_DeleteRow(FExecuteAction::CreateRaw(this, &STree::DeleteRow, Item));
		return SNew(STableRow<FTreeItemPtr>, OwnerTable)
		.OnDragDetected(this, &STree::DragDetected, ItemText)
		.OnCanAcceptDrop(this, &STree::OnCanAcceptDrop)
		.OnAcceptDrop(this, &STree::OnAcceptDrop)
		[
			SNew(SVRDebugSuggestionTextBox)
			.Text(ItemText)
			.IsNewCommand(Item->IsCommandNew())
			.SuggestionListMaxHeight(200.0f)
			.BackgroundColor(FLinearColor(0, 0, 0, 0))
			.ForegroundColor(Item->IsCommandNew() ? FLinearColor(0, 1, 0, 1) : FLinearColor(1, 1, 1, 1))
			.OnShowingSuggestions(FOnShowingSuggestions::CreateRaw(this, &STree::OnShowingSuggestion))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"), 14))
			.DeleteRowAction(Action_DeleteRow)
			.OnTextChanged(FOnTextChanged::CreateRaw(this, &STree::OnCommandTextChanged, Item))
		];
	}
	else
	{
		return SNew(STableRow< FTreeItemPtr >, OwnerTable)
		.OnDragDetected(this, &STree::DragDetected, ItemText)
		.OnCanAcceptDrop(this, &STree::OnCanAcceptDrop)
		.OnAcceptDrop(this, &STree::OnAcceptDrop)
		[
			SNew(SBorder)
			.BorderImage(FVRDebugPluginStyle::Get().GetBrush("WhiteBackground"))
			.BorderBackgroundColor(FLinearColor(0.029557, 0.029557, 0.029557, 1))
			[
				SNew(STextBlock)
				.Text(ItemText)
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
				.ColorAndOpacity(FLinearColor(1, 1, 1, 1))
			]
		];
	}
}

void STree::Tree_OnGetChildren(FTreeItemPtr Item, TArray<FTreeItemPtr>& OutChildren)
{
	const auto& Commands = Item->GetCommands();
	OutChildren.Append(Commands);
}

void STree::Tree_OnSelectionChanged(FTreeItemPtr Item, ESelectInfo::Type SelectInfo)
{
	if (Item.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Item Selected: %s"), *Item->GetDisplayName());
	}
}

void STree::OnShowingSuggestion(const FString& NewText, TArray<FString>& OutSuggestions)
{
	for (int32 i = 0; i < EditorConsoleCommands.Num(); i++)
	{
		if (EditorConsoleCommands[i].StartsWith(NewText)) OutSuggestions.Add(EditorConsoleCommands[i]);
	}
}

void STree::DeleteRow(FTreeItemPtr Item)
{
	if (!Item.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Tree Item not valid inside delete row function"));
	}

	int32 CatIndex = -1;

	for (int32 i = 0; i < Categories.Num(); i++)
	{
		for (int32 k = 0; k < Categories[i]->GetCommands().Num(); k++)
		{
			if (Categories[i]->GetCommands()[k] == Item)
			{
				CatIndex = i;
			}
		}
	}

	if (CatIndex != -1)
	{
		Categories[CatIndex]->AccessCommands().Remove(Item);
	}

	if (Categories[CatIndex]->GetCommands().Num() == 0)
	{
		DeleteCategory(Categories[CatIndex]);
	}

	if (TreeView.IsValid())
	{
		TreeView->RequestTreeRefresh();
	}
}

void STree::OnCommandTextChanged(const FText& InText, FTreeItemPtr Item)
{
	Item->SetCommandText(InText);
}

void STree::AddNewCommand(const FText& InText)
{
	if (TreeView.IsValid())
	{
		if (TreeView->GetSelectedItems().Num() > 0)
		{
			FTreeItemPtr ItemSelected = TreeView->GetSelectedItems()[0];
			if (ItemSelected->IsCategory())
			{
				TSharedRef<FTreeItem> Command = MakeShareable(new FTreeItem(ItemSelected, InText.ToString(), true));
				ItemSelected->AddCommand(Command);
				TreeView->SetItemExpansion(ItemSelected, true);
				if (TreeView.IsValid())
				{
					TreeView->RequestTreeRefresh();
				}
			}
		}
	}
}

void STree::AddNewCategory(const FText& InText)
{
	TSharedRef<FTreeItem> Category = MakeShareable(new FTreeItem(NULL, InText.ToString()));
	Categories.Insert(Category, 0);
	const FText& NewCommandText = FText::FromString("New Command");
	TSharedRef<FTreeItem> Command = MakeShareable(new FTreeItem(Category, NewCommandText.ToString(), true));
	Category->AddCommand(Command);
	if (TreeView.IsValid())
	{
		TreeView->RequestTreeRefresh();
	}
}

void STree::DeleteCategory(const FTreeItemPtr Item)
{
	if (TreeView.IsValid())
	{
		FTreeItemPtr ItemSelected;
		if (Item.IsValid())
		{
			ItemSelected = Item;
		}
		else if (TreeView->GetSelectedItems().Num() > 0)
		{
			ItemSelected = TreeView->GetSelectedItems()[0];
		}

		if (ItemSelected.IsValid() && ItemSelected->IsCategory())
		{
			Categories.Remove(ItemSelected);
			TreeView->RequestTreeRefresh();
		}
	}
}

TMap<FString, TArray<FString>> STree::GetEditedConsoleCommands()
{
	CategoryCommandsList.Empty();

	for (int32 i = Categories.Num() - 1; i >=0; i--)
	{
		CategoryCommandsList.Add(Categories[i]->GetDisplayName(), TArray<FString>());
		for (auto Command : Categories[i]->GetCommands())
		{
			CategoryCommandsList.Find(Categories[i]->GetDisplayName())->Add(Command->GetDisplayName());
		}
	}

	return CategoryCommandsList;
}

TOptional<EItemDropZone> STree::OnCanAcceptDrop(const FDragDropEvent& InDragDropEvent, EItemDropZone InDropZone, FTreeItemPtr Item)
{
	switch (InDropZone)
	{
	case EItemDropZone::AboveItem:
		return InDropZone;
		break;
	case EItemDropZone::BelowItem:
		return InDropZone;
		break;
	case EItemDropZone::OntoItem:
		return EItemDropZone::AboveItem;
		break;
	default:
		return EItemDropZone::AboveItem;
		break;
	}
}

FReply STree::DragDetected(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FText ItemText)
{
	TSharedRef<FTreeDragDropOp> Operation = FTreeDragDropOp::New(ItemText.ToString());
	return FReply::Handled().BeginDragDrop(Operation);
}

FReply STree::OnAcceptDrop(const FDragDropEvent& InDragDropEvent, EItemDropZone InDropZone, FTreeItemPtr Item)
{
	FText ItemDropped = InDragDropEvent.GetOperationAs<FTreeDragDropOp>()->GetText();
	FTreeItemPtr ItemDroppedPtr;
	for (auto item : Categories)
	{
		for (auto SubItem : item->AccessCommands())
		{
			if (SubItem->GetDisplayName() == ItemDropped.ToString())
			{
				ItemDroppedPtr = SubItem;
			}
		}

		if (item->GetDisplayName() == ItemDropped.ToString())
		{
			ItemDroppedPtr = item;
		}
	}

	if (!ItemDroppedPtr.IsValid() || !Item.IsValid())
		return FReply::Unhandled();

	if(ItemDroppedPtr == Item)
		return FReply::Unhandled();

	if (ItemDroppedPtr->IsCategory() && Item->IsCategory())
	{
		DropCategory(ItemDroppedPtr, Item, InDropZone);
		if (TreeView.IsValid())
		{
			TreeView->RequestTreeRefresh();
		}
		return FReply::Handled();
	}

	if (!ItemDroppedPtr->IsCategory() && !Item->IsCategory())
	{
		DropCommand(ItemDroppedPtr, Item, InDropZone);
		if (TreeView.IsValid())
		{
			TreeView->RequestTreeRefresh();
		}
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void STree::DropCategory(FTreeItemPtr ItemDropped, FTreeItemPtr Item, EItemDropZone ItemDropZone)
{
	Categories.Remove(ItemDropped);
	int32 index = -1;
	for (int32 i = 0; i < Categories.Num(); i++)
	{
		if (Categories[i] == Item)
		{
			index = i;
		}
	}

	if (ItemDropZone == EItemDropZone::AboveItem)
	{
		if (index != -1)
		{
			if (index - 1 < 0)
				Categories.Insert(ItemDropped, 0);
			else
				Categories.Insert(ItemDropped, index);
		}
	}
	else
	{
		if (index != -1)
		{
			if (index == Categories.Num() - 1)
				Categories.Add(ItemDropped);
			else
				Categories.Insert(ItemDropped, index + 1);
		}
	}
}

void STree::DropCommand(FTreeItemPtr ItemDropped, FTreeItemPtr Item, EItemDropZone ItemDropZone)
{
	int32 CatIndex = -1;
	int32 SubIndex = -1;
	for (int32 k = 0; k < Categories.Num(); k++)
	{
		for (int32 i = 0; i < Categories[k]->AccessCommands().Num(); i++)
		{
			if (Categories[k]->AccessCommands()[i] == ItemDropped)
			{
				CatIndex = k;
				SubIndex = i;
			}
		}
	}

	if (CatIndex != -1 && SubIndex != -1)
	{
		Categories[CatIndex]->AccessCommands().Remove(ItemDropped);
	}

	for (int32 k = 0; k < Categories.Num(); k++)
	{
		for (int32 i = 0; i < Categories[k]->AccessCommands().Num(); i++)
		{
			if (Categories[k]->AccessCommands()[i] == Item)
			{
				CatIndex = k;
				SubIndex = i;
			}
		}
	}

	if (ItemDropZone == EItemDropZone::AboveItem)
	{
		if (SubIndex != -1)
		{
			if (SubIndex - 1 < 0)
				Categories[CatIndex]->AccessCommands().Insert(ItemDropped, 0);
			else
				Categories[CatIndex]->AccessCommands().Insert(ItemDropped, SubIndex);
		}
	}
	else
	{
		if (SubIndex != -1)
		{
			if (SubIndex == Categories[CatIndex]->AccessCommands().Num() - 1)
				Categories[CatIndex]->AccessCommands().Add(ItemDropped);
			else
				Categories[CatIndex]->AccessCommands().Insert(ItemDropped, SubIndex + 1);
		}
	}
}

