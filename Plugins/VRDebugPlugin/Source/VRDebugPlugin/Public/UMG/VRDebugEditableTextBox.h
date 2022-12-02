// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "VRDebugEditableTextBox.generated.h"

UCLASS()
class VRDEBUGPLUGIN_API UVRDebugEditableTextBox : public UEditableTextBox
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "VR Debug Editable TextBox")
	void SetCaretToEnd();
};
