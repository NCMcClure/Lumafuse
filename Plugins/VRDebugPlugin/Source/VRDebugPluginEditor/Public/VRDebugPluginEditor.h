// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConsoleCommandEditorTool/ConsoleCommandTab.h"
#include "Modules/ModuleManager.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyle.h"

class FToolBarBuilder;
class FMenuBuilder;

class FVRDebugPluginEditorModule : public IModuleInterface
{
public:
	TSharedPtr<FSlateStyleSet> StyleSet;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	void AddToolBarExtension(FToolBarBuilder& Builder);
	void CreateSubMenu(FMenuBuilder& MenuBuilder) const;

	TSharedRef<class SDockTab> OnSpawnPluginCommandsEditorTab(const class FSpawnTabArgs& SpawnTabArgs);
private:

	FConsoleCommandTab* ConsoleCommandTab;
	TSharedPtr<class FUICommandList> PluginCommands;
};
