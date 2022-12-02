// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Log.h"
#include "DebugWidgetView.h"
#include "OutputLog.generated.h"

UCLASS()
class VRDEBUGPLUGIN_API UOutputLog : public UDebugWidgetView
{
	GENERATED_BODY()

public:

	UOutputLog(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, Category = "OutputLog")
	TArray<FVRDebugLogMessage> Messages;

	UFUNCTION(BlueprintCallable, Category = "OutputLog")
	void ForceGetMessage();

	UFUNCTION(BlueprintImplementableEvent, Category = "OutputLog")
	void OutputLogUpdated();

	UFUNCTION(BlueprintPure, Category = "OutputLog")
	FString GetMessage(FVRDebugLogMessage  Message);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OutputLog")
	int32 MaxLineLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OutputLog")
	int32 MaxStoredMessages;

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TArray<TSharedPtr<FVRDebugLogMessage>> Messages_Internal;

	void ConvertMessages();
};
