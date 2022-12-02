// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "ConsoleCommandEditorTool/ConsoleCommandToolbar.h"
#include "Widgets/Docking/SDockTab.h"
#include "CommandsTreeView.h"

class FConsoleCommandTab
{
public:
	FConsoleCommandTab();
	TSharedRef<SDockTab> GetConsoleCommandsEditorTab();

private:
	const FString CommandListConfigName = "Commands.ini";
	TSharedPtr<STree> CommandTreeView;
	FToolbar* Toolbar;

private:
	FReply DeleteCategory();
	FReply SaveCommands();
	FReply OpenCommandsFile();
	FReply OpenCommandFileFolder();
	FReply AddNewCategory();
	FReply AddNewCommand();
};


