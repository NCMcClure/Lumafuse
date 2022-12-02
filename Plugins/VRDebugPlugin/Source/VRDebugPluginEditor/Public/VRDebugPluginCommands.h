// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "VRDebugPluginStyle.h"

/**
 *
 */
class FVRDebugPluginCommands : public TCommands<FVRDebugPluginCommands>
{
public:

	FVRDebugPluginCommands()
		: TCommands<FVRDebugPluginCommands>(TEXT("VRDebugPlugin"), NSLOCTEXT("Contexts", "VRDebugPlugin", "VRDebugPlugin Plugin"), NAME_None, FVRDebugPluginStyle::GetStyleSetName())
	{}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenVRDebugCommandsEditor;
	TSharedPtr<FUICommandInfo> OpenVRDebugInfo;
};
