// Copyright 2021 DownToCode. All Rights Reserved.

#include "UMG/Log.h"

FLogHistory ULogFL::Log;

FLogHistory::FLogHistory()
{
	MaxLineLength = 130;
	bOutputLogIsDirty = false;
	bPrintLogIsDirty = false;
	MaxStoredMessages = 1000;
	GLog->AddOutputDevice(this);
	GLog->SerializeBacklog(this);
}

FLogHistory::~FLogHistory()
{
	// At shutdown, GLog may already be null
	if (GLog != NULL)
	{
		GLog->RemoveOutputDevice(this);
	}
}

const TArray<TSharedPtr<FVRDebugLogMessage>>& FLogHistory::GetOutputLogMessages()
{
	return OutlogMessages;
}

const TArray<TSharedPtr<FVRDebugLogMessage>>& FLogHistory::GetPrintMessages()
{
	return PrintMessages;
}

void FLogHistory::EmptyPrintMessages()
{
	PrintMessages.Empty();
}

void FLogHistory::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category)
{
	// Capture all incoming OutlogMessages and store them in history
	CreateLogMessages(V, Verbosity, Category, OutlogMessages, PrintMessages);
}

bool FLogHistory::CreateLogMessages(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category, TArray<TSharedPtr<FVRDebugLogMessage>>& OutMessages, TArray<TSharedPtr<FVRDebugLogMessage>>& OutPrintMessages)
{
	// Skip Color Events
	if (Verbosity == ELogVerbosity::SetColor) return false;
	else
	{
		FName Style;
		if (Category == NAME_Cmd)
		{
			Style = FName(TEXT("Log.Command"));
		}
		else if (Verbosity == ELogVerbosity::Error)
		{
			Style = FName(TEXT("Log.Error"));
		}
		else if (Verbosity == ELogVerbosity::Warning)
		{
			Style = FName(TEXT("Log.Warning"));
		}
		else
		{
			Style = FName(TEXT("Log.Normal"));
		}

		// Determine how to format timestamps
		static ELogTimes::Type LogTimestampMode = ELogTimes::None;
		const int32 OldNumMessages = OutMessages.Num();

		// handle multiline strings by breaking them apart by line
		TArray<FTextRange> LineRanges;
		const FString CurrentLogDump = V;
		FTextRange::CalculateLineRangesFromString(CurrentLogDump, LineRanges);

		bool bIsFirstLineInMessage = true;
		bool bIsBlueprintLogLine = false;
		for (const FTextRange& LineRange : LineRanges)
		{
			if (!LineRange.IsEmpty())
			{
				FString Line = CurrentLogDump.Mid(LineRange.BeginIndex, LineRange.Len());
				Line = Line.ConvertTabsToSpaces(4);

				// Hard-wrap lines to avoid them being too long
				const int32 HardWrapLen = MaxLineLength;
				for (int32 CurrentStartIndex = 0; CurrentStartIndex < Line.Len();)
				{
					int32 HardWrapLineLen = 0;
					if (bIsFirstLineInMessage)
					{
						FString MessagePrefix = FOutputDeviceHelper::FormatLogLine(Verbosity, Category, nullptr, LogTimestampMode);

						HardWrapLineLen = FMath::Min(HardWrapLen - MessagePrefix.Len(), Line.Len() - CurrentStartIndex);
						FString HardWrapLine = Line.Mid(CurrentStartIndex, HardWrapLineLen);
						FString FinalLine = FString(MessagePrefix + HardWrapLine);
						
						OutMessages.Add(MakeShareable(new FVRDebugLogMessage(MakeShareable(new FString(FinalLine)), Verbosity, Category, Style)));

						if(FinalLine.Contains("DFLLog: DFL"))
						{
							const FString PrintLine = FinalLine.RightChop(11);
							OutPrintMessages.Add(MakeShareable(new FVRDebugLogMessage(MakeShareable(new FString(PrintLine)), Verbosity, Category, Style)));
							bIsBlueprintLogLine = true;
						}
						
						if (FinalLine.Contains("LogBlueprintUserMessages: ["))
						{
							int32 Pos;
							FinalLine.FindChar('[', Pos);
							if (Pos != INDEX_NONE)
							{
								const FString PrintLine = FinalLine.RightChop(Pos);
								OutPrintMessages.Add(MakeShareable(new FVRDebugLogMessage(MakeShareable(new FString(PrintLine)), Verbosity, Category, Style)));
								bIsBlueprintLogLine = true;
							}
						}
					}
					else
					{
						HardWrapLineLen = FMath::Min(HardWrapLen, Line.Len() - CurrentStartIndex);
						FString HardWrapLine = Line.Mid(CurrentStartIndex, HardWrapLineLen);
						const TSharedRef<FString> HardWrapLineShared = MakeShareable(new FString(MoveTemp(HardWrapLine)));

						if(bIsBlueprintLogLine) OutPrintMessages.Add(MakeShareable(new FVRDebugLogMessage(HardWrapLineShared, Verbosity, Category, Style)));
						OutMessages.Add(MakeShareable(new FVRDebugLogMessage(HardWrapLineShared, Verbosity, Category, Style)));
					}

					bIsFirstLineInMessage = false;
					CurrentStartIndex += HardWrapLineLen;
				}
			}
		}

		const int NumMessages = OutMessages.Num();
		if (NumMessages > MaxStoredMessages)
		{
			OutMessages.RemoveAt(0, NumMessages - MaxStoredMessages, true);
		}

		if (OldNumMessages != NumMessages)
		{
			bOutputLogIsDirty = true;
			bPrintLogIsDirty = true;
		}

		return OldNumMessages != NumMessages;
	}
}

