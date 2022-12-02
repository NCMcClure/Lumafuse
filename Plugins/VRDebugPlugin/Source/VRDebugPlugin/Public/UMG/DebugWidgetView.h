// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"
#include "LaserBeam.h"
#include "DebugWidgetView.generated.h"

class UVRDebugComponent;

UCLASS()
class VRDEBUGPLUGIN_API UDebugWidgetView : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UDebugWidgetView(const FObjectInitializer& ObjectInitializer);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Widget View")
	bool bEmptyTextboxOnClick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Widget View")
	FText ValueOfLastSelectedTextbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category ="Debug Widget View")
	UVRDebugComponent* VRDebugComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "Debug Widget View")
	AActor* VRDebugActor;

	UEditableTextBox* LastSelectedTextBox;

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Debug Widget View")
	void Init();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Debug Widget View")
	void ReceivedFocus();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Debug Widget View")
	void LostFocus();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Debug Widget View")
	void SpawnKeyboard();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Debug Widget View")
	void WidgetShown();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Debug Widget View")
	void WidgetHidden();

	UFUNCTION(BlueprintCallable, Category = "Debug Widget View")
	void ResetLastSelectedTextBox();
protected:

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void StopLaserPointer(ALaserBeam* LaserBeam);

};
