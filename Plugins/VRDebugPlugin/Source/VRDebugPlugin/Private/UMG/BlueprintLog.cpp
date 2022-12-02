// Copyright 2021 DownToCode. All Rights Reserved.

#include "UMG/BlueprintLog.h"

UBlueprintLog::UBlueprintLog(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
}

void UBlueprintLog::NativeConstruct()
{
	ULogFL::Log.MaxStoredMessages = FMath::Clamp(MaxStoredMessages, 100, 100000);
	ULogFL::Log.MaxLineLength = FMath::Clamp(MaxLineLength, 50, 1000);

	Super::NativeConstruct();
}

void UBlueprintLog::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (ULogFL::Log.bPrintLogIsDirty)
	{
		Messages_Internal = ULogFL::Log.GetPrintMessages();
		ConvertMessages();
		OutputLogUpdated();
		Messages_Internal.Empty();
		ULogFL::Log.EmptyPrintMessages();
		ULogFL::Log.bPrintLogIsDirty = false;
	}
}

void UBlueprintLog::NativeDestruct()
{
	Super::NativeDestruct();
	ULogFL::Log.EmptyPrintMessages();
}

void UBlueprintLog::ConvertMessages()
{
	Messages.Empty();

	for (TSharedPtr<FVRDebugLogMessage> Message : Messages_Internal)
	{
		Messages.Add(*Message.Get());
	}
}

FString UBlueprintLog::GetMessage(FVRDebugLogMessage& Message)
{
	return Message.Message.Get();
}