// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

typedef TSharedPtr<class FTreeItem> FTreeItemPtr;

class FTreeItem
{

public:
	void SetCommandText(const FText& InText)
	{
		CommandText = InText.ToString();
	}

	const FString& GetDisplayName() const
	{
		return CommandText;
	}

	const TArray<FTreeItemPtr>& GetCommands() const
	{
		return Commands;
	}

	TArray<FTreeItemPtr>& AccessCommands()
	{
		return Commands;
	}

	void AddCommand(const FTreeItemPtr NewCommand)
	{
		Commands.Add(NewCommand);
	}

	bool IsCategory()
	{
		return !Category.IsValid();
	}

	bool IsCommandNew()
	{
		if (!IsCategory())
		{
			return bIsNew;
		}
		else return false;
	}

	void SetIsNewCommand(bool bNewCommand)
	{
		if (!IsCategory())
		{
			bIsNew = bNewCommand;
		}
	}

public:

	/** Constructor for FDDFileTreeItem */
	FTreeItem(const FTreeItemPtr InCategory, const FString& InCommandText, bool InNew = false)
		: Category(InCategory)
		, CommandText(InCommandText)
		, bIsNew(InNew)
	{}

private:

	TWeakPtr<FTreeItem> Category;
	FString CommandText;
	bool bIsNew = false;
	TArray<FTreeItemPtr> Commands;
};