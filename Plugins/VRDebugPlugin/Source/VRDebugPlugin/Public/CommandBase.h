// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "CommandBase.generated.h"

UCLASS(Blueprintable)
class VRDEBUGPLUGIN_API UCommandBase : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Command Base")
	void Init();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Command Base")
	void PerformAction();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Command Base")
	void Undo();
};
