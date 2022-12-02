// Copyright 2021 DownToCode. All Rights Reserved.

#include "UMG/OutputLog.h"

UOutputLog::UOutputLog(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
}

void UOutputLog::NativeConstruct()
{
	ULogFL::Log.MaxStoredMessages = FMath::Clamp(MaxStoredMessages, 100, 100000);
	ULogFL::Log.MaxLineLength = FMath::Clamp(MaxLineLength, 50, 1000);

	Super::NativeConstruct();
}

void UOutputLog::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (ULogFL::Log.bOutputLogIsDirty)
	{
		Messages_Internal = ULogFL::Log.GetOutputLogMessages();
		ConvertMessages();
		OutputLogUpdated();
		ULogFL::Log.bOutputLogIsDirty = false;
	}
}

void UOutputLog::ConvertMessages()
{
	Messages.Empty();

	for (TSharedPtr<FVRDebugLogMessage> Message : Messages_Internal)
	{
		Messages.Add(*Message.Get());
	}
}

void UOutputLog::ForceGetMessage()
{
	Messages_Internal = ULogFL::Log.GetOutputLogMessages();
	ConvertMessages();
	OutputLogUpdated();
	ULogFL::Log.bOutputLogIsDirty = false;
}

FString UOutputLog::GetMessage(FVRDebugLogMessage Message)
{
	return Message.Message.Get();
}