// Copyright 2019 DownToCode. All Rights Reserved.

#include "VRDebugPluginCommands.h"

#define LOCTEXT_NAMESPACE "FVRDebugPluginEditorModule"

void FVRDebugPluginCommands::RegisterCommands()
{
	UI_COMMAND(OpenVRDebugCommandsEditor, "Console Commands Editor", "Opens the VR Debug Plugin Commands Editor", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(OpenVRDebugInfo, "VR Debug Plugin Info", "Opens the VR Debug Plugin Info", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE