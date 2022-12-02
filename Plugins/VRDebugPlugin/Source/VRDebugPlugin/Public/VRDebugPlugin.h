// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

class FVRDebugPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	TArray<FString> DefaultCommands;
};
