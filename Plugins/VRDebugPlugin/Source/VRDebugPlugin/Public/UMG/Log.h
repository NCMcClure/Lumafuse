// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 23
#include "Runtime/Slate/Public/Framework/Text/TextRange.h"
#endif
#include "Core/Public/Misc/OutputDeviceHelper.h"
#include "Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h"
#include "Log.generated.h"

USTRUCT(BlueprintType)
struct FVRDebugLogMessage
{
	GENERATED_USTRUCT_BODY()

	TSharedRef<FString> Message;
	ELogVerbosity::Type Verbosity;

	UPROPERTY(BlueprintReadOnly, Category = "VR Log Message")
	FName Category;

	UPROPERTY(BlueprintReadOnly, Category = "VR Log Message")
	FName Style;

	FVRDebugLogMessage()
		: Message(MakeShareable(new FString()))
		, Verbosity(ELogVerbosity::Log)
		, Category(TEXT(""))
		, Style(TEXT(""))
	{
	}

	FVRDebugLogMessage(const TSharedRef<FString>& NewMessage, FName NewCategory, FName NewStyle = NAME_None)
		: Message(NewMessage)
		, Verbosity(ELogVerbosity::Log)
		, Category(NewCategory)
		, Style(NewStyle)
	{
	}

	FVRDebugLogMessage(const TSharedRef<FString>& NewMessage, ELogVerbosity::Type NewVerbosity, FName NewCategory, FName NewStyle = NAME_None)
		: Message(NewMessage)
		, Verbosity(NewVerbosity)
		, Category(NewCategory)
		, Style(NewStyle)
	{
	}
};

class FLogHistory : public FOutputDevice
{
public:

	bool bOutputLogIsDirty;
	bool bPrintLogIsDirty;
	int32 MaxStoredMessages;
	int32 MaxLineLength;

	FLogHistory();
	~FLogHistory();

	/** Gets all captured output log messages */
	const TArray<TSharedPtr<FVRDebugLogMessage>>& GetOutputLogMessages();
	/** Get all captured print messages */
	const TArray<TSharedPtr<FVRDebugLogMessage>>& GetPrintMessages();
	/** Empty the print messages as we want to only show the once for each time we run the app/game */
	void EmptyPrintMessages();
protected:

	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category) override;

	bool CreateLogMessages(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category, TArray< TSharedPtr<FVRDebugLogMessage> >& OutMessages, TArray<TSharedPtr<FVRDebugLogMessage>>& OutPrintMessages);
	
private:
	/** All log messages since this module has been started */
	TArray<TSharedPtr<FVRDebugLogMessage>> OutlogMessages;

	/** All print messages since this module has been started */
	TArray<TSharedPtr<FVRDebugLogMessage>> PrintMessages;
};

UCLASS()
class VRDEBUGPLUGIN_API ULogFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FLogHistory Log;
};
