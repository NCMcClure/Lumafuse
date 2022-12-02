// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "DebugWidgetView.h"
#include "ConsoleWidget.generated.h"

USTRUCT(BlueprintType)
struct FConsoleCommandList
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Console Command List")
	FString ListName;

	UPROPERTY(BlueprintReadWrite, Category = "Console Command List")
	TArray<FString> List;
};

UCLASS()
class VRDEBUGPLUGIN_API UConsoleWidget : public UDebugWidgetView
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FString CommandListConfigName = "Commands.ini";

public:
	UFUNCTION(BlueprintCallable, Category = "Console Command List")
	TArray<FConsoleCommandList> GetCommandList(bool bForceParse, bool bOnAndroid = false);

	UFUNCTION(BlueprintCallable, Category = "Console Widget")
	void AddConsoleCommandsToIniFile(FString Category, TArray<FString> Commands, bool bReloadConfig);

	UFUNCTION(BlueprintCallable, Category = "Console Widget")
	void AddConsoleCommandToIniFile(FString Command, FString CommandCategory);

	UFUNCTION(BlueprintCallable, Category = "Console Widget")
	void RemoveConsoleCommandFromIniFile(FString Command, FString CommandCategory);

	UFUNCTION(BlueprintCallable, Category = "Text size")
	static void GetTextSize(UFont* Font, FString Text, float& Width, float& Height);

public:
	UPROPERTY()
	TArray<FConsoleCommandList> ConsoleCommandList;

	UFUNCTION()
	void ParseCommandList(bool bOnAndroid = false);

	UPROPERTY()
	int32 SectionIndex;

	void OnEntry(const TCHAR* Key, const TCHAR* Value);
};

class FGetValue
{
public:
	const FString& IniFilename;
	int32 SectionIndex;
	TArray<FConsoleCommandList>& ConsoleCommandList;

	FGetValue(const FString& InIniFilename, TArray<FConsoleCommandList>& InConsoleCommandList, int32 InSectionIndex)
		: IniFilename(InIniFilename)
		, SectionIndex(InSectionIndex)
		, ConsoleCommandList(InConsoleCommandList)
	{}

	void OnEntry(const TCHAR* Key, const TCHAR* Value)
	{
		if (Key)
		{
			if (ConsoleCommandList.IsValidIndex(SectionIndex))
			{
				ConsoleCommandList[SectionIndex].List.Add(Key);
			}
		}
		else return;
	}
};
