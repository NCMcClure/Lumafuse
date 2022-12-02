// Copyright 2019 DownToCode. All Rights Reserved.

#include "ConsoleCommandEditorTool/ConsoleCommandTab.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Projects/Public/Interfaces/IPluginManager.h"
#include "ConsoleCommandEditorTool/ConsoleCommandHelper.h"

#define LOCTEXT_NAMESPACE "VRDebugPluginTabs"

FConsoleCommandTab::FConsoleCommandTab()
{
}

TSharedRef<SDockTab> FConsoleCommandTab::GetConsoleCommandsEditorTab()
{
	FOnClicked AddCategory = FOnClicked::CreateRaw(this, &FConsoleCommandTab::AddNewCategory);
	FOnClicked AddCommand = FOnClicked::CreateRaw(this, &FConsoleCommandTab::AddNewCommand);
	FOnClicked DeleteCategory = FOnClicked::CreateRaw(this, &FConsoleCommandTab::DeleteCategory);
	FOnClicked SaveCommands = FOnClicked::CreateRaw(this, &FConsoleCommandTab::SaveCommands);
	FOnClicked OpenCommandsFile = FOnClicked::CreateRaw(this, &FConsoleCommandTab::OpenCommandsFile);
	FOnClicked OpenCommandFileFolder = FOnClicked::CreateRaw(this, &FConsoleCommandTab::OpenCommandFileFolder);
	Toolbar = new FToolbar(AddCategory, AddCommand, DeleteCategory, SaveCommands, OpenCommandsFile, OpenCommandFileFolder);

	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			[
				Toolbar->GetReference()
			]
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		+ SVerticalBox::Slot()
		.FillHeight(0.8f)
		[
			SNew(SBorder)
			[
				SNew(SScrollBox)
				.Orientation(EOrientation::Orient_Vertical)
				+SScrollBox::Slot()
				[
					SAssignNew(CommandTreeView, STree)
				]
			]
		]
	];
}

FReply FConsoleCommandTab::AddNewCategory()
{
	const FText& CategoryName = FText::FromString(Toolbar->GetNewCategoryName());
	CommandTreeView->AddNewCategory(CategoryName);
	Toolbar->EmptyNewCategoryTextBox();
	return FReply::Handled();
}

FReply FConsoleCommandTab::AddNewCommand()
{
	const FText& NewCommand = FText::FromString("New Command");
	CommandTreeView->AddNewCommand(NewCommand);
	return FReply::Handled();
}

FReply FConsoleCommandTab::DeleteCategory()
{
	CommandTreeView->DeleteCategory();
	return FReply::Handled();
}

FReply FConsoleCommandTab::SaveCommands()
{
	FConsoleCommandHelper::SaveConsoleCommandsIniFile(CommandTreeView->GetEditedConsoleCommands());
	CommandTreeView->RebuildTree();
	return FReply::Handled();
}

FReply FConsoleCommandTab::OpenCommandsFile()
{
	FString ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config");
	FString ConfigFileFullPath = ConfigFilePath / CommandListConfigName;
	FPlatformProcess::LaunchFileInDefaultExternalApplication(*FPaths::ConvertRelativePathToFull(ConfigFileFullPath));
	return FReply::Handled();
}

FReply FConsoleCommandTab::OpenCommandFileFolder()
{
	FString ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config");
	FString ConfigFileFullPath = ConfigFilePath / CommandListConfigName;
	FPlatformProcess::ExploreFolder(*ConfigFileFullPath);
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE