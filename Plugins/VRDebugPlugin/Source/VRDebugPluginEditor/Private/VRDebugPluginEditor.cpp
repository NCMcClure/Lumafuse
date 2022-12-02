// Copyright 2019 DownToCode. All Rights Reserved.

#include "VRDebugPluginEditor.h"
#include "VRDebugDetails.h"
#include "LevelEditor.h"
#include "VRDebugPluginCommands.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"

static const FName VRDebugPluginCommandsEditorTabName("VRDebugPluginCommandsMenu");
static const FName VRDebugPluginInfoTabName("VRDebugPluginInfo");

#define LOCTEXT_NAMESPACE "FVRDebugPluginEditorModule"

void FVRDebugPluginEditorModule::StartupModule()
{
	ConsoleCommandTab = new FConsoleCommandTab();

	// Register the VR Debug Component Custom class layout
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("VRDebugComponent", FOnGetDetailCustomizationInstance::CreateStatic(&FVRDebugDetails::MakeInstance));

	//Register the VRDebugPluginCommands
	FVRDebugPluginCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList());

	//Initialize and reload textures from VRDebugPluginStyle
	FVRDebugPluginStyle::Initialize();
	FVRDebugPluginStyle::ReloadTextures();

	//Add button for VR Debug Plugin menu in toolbar
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender());
#if ENGINE_MAJOR_VERSION == 5
		ToolbarExtender->AddToolBarExtension("Play", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FVRDebugPluginEditorModule::AddToolBarExtension));
#else
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FVRDebugPluginEditorModule::AddToolBarExtension));
#endif
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	//Register nomad tabs
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(VRDebugPluginCommandsEditorTabName, FOnSpawnTab::CreateRaw(this, &FVRDebugPluginEditorModule::OnSpawnPluginCommandsEditorTab))
		.SetDisplayName(LOCTEXT("VRDebugPluginCommnadsEditorTabTitle", "VR Debug Commands Editor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FVRDebugPluginEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

TSharedRef<SDockTab> FVRDebugPluginEditorModule::OnSpawnPluginCommandsEditorTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return ConsoleCommandTab->GetConsoleCommandsEditorTab();
}

void FVRDebugPluginEditorModule::AddToolBarExtension(FToolBarBuilder& Builder)
{
	auto ConstructPluginContextMenu = [this]()
	{
		FMenuBuilder MenuBuilder(true, nullptr);

		MenuBuilder.AddMenuEntry(
			FVRDebugPluginCommands::Get().OpenVRDebugCommandsEditor->GetLabel(),
			FVRDebugPluginCommands::Get().OpenVRDebugCommandsEditor->GetDescription(),
			FSlateIcon(),
#if (ENGINE_MAJOR_VERSION == 5) || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 25)
			FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(VRDebugPluginCommandsEditorTabName); })));
#else
			FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->InvokeTab(VRDebugPluginCommandsEditorTabName); })));
#endif

		MenuBuilder.AddSubMenu(
			LOCTEXT("Info", "Info"),
			LOCTEXT("InfoTooltip", "Tutorials info & more"),
			FNewMenuDelegate::CreateRaw(this, &FVRDebugPluginEditorModule::CreateSubMenu)
		);

		return MenuBuilder.MakeWidget();
	};

	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateLambda(ConstructPluginContextMenu),
		FText::FromString("VRDebugMenu"),
		FText::FromString("VR Debug Plugin Menu"),
		FSlateIcon("FVRDebugPluginStyle", "ToolbarButton")
	);
}

void FVRDebugPluginEditorModule::CreateSubMenu(FMenuBuilder& MenuBuilder) const
{
	FUIAction DiscordChannelAction = FUIAction(FExecuteAction::CreateLambda([]() {
		FPlatformProcess::LaunchURL(*FString("https://discord.gg/cueDj9"), nullptr, nullptr);
	}));

	FUIAction UE4ForumAction = FUIAction(FExecuteAction::CreateLambda([]() {
		FPlatformProcess::LaunchURL(*FString("https://forums.unrealengine.com/development-discussion/vr-ar-development/1668459-vr-debug-plugin"), nullptr, nullptr);
	}));

	FUIAction WhatsNewAction = FUIAction(FExecuteAction::CreateLambda([]() {
		FPlatformProcess::LaunchURL(*FString("https://drive.google.com/open?id=1Q4c6oqGIZHjzytLk-xduTpPJf2DZRWIT"), nullptr, nullptr);
	}));

	FUIAction TutorialsAction = FUIAction(FExecuteAction::CreateLambda([]() {
		FPlatformProcess::LaunchURL(*FString("https://www.downtocode.com/vrdebugplugin"), nullptr, nullptr);
	}));

	MenuBuilder.AddMenuEntry(FText::FromString("Tutorials"), FText::GetEmpty(), FSlateIcon(), TutorialsAction);
	MenuBuilder.AddMenuEntry(FText::FromString("UE4 Forum"), FText::GetEmpty(), FSlateIcon(), UE4ForumAction);
	MenuBuilder.AddMenuEntry(FText::FromString("Discord Channel"), FText::GetEmpty(), FSlateIcon(), DiscordChannelAction);
	MenuBuilder.AddMenuEntry(FText::FromString("What's new"), FText::GetEmpty(), FSlateIcon(), WhatsNewAction);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVRDebugPluginEditorModule, VRDebugPluginEditor)