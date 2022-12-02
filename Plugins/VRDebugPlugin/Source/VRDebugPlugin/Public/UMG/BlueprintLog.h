// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Log.h"
#include "DebugWidgetView.h"
#include "BlueprintLog.generated.h"

UCLASS()
class VRDEBUGPLUGIN_API UBlueprintLog : public UDebugWidgetView
{
	GENERATED_BODY()

public:

	UBlueprintLog(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, Category = "OutputLog")
	TArray<FVRDebugLogMessage> Messages;

	UFUNCTION(BlueprintImplementableEvent)
	void OutputLogUpdated();

	UFUNCTION(BlueprintPure, Category = "OutputLog")
	FString GetMessage(UPARAM(Ref)FVRDebugLogMessage& Message);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OutputLog")
	int32 MaxLineLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OutputLog")
	int32 MaxStoredMessages;

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeDestruct() override;
private:
	TArray<TSharedPtr<FVRDebugLogMessage>> Messages_Internal;

	void ConvertMessages();
};
