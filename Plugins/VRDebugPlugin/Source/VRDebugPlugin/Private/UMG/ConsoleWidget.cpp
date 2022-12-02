// Copyright 2021 DownToCode. All Rights Reserved.

#include "UMG/ConsoleWidget.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#include "VRDebugLog.h"

TArray<FConsoleCommandList> UConsoleWidget::GetCommandList(bool bForceParse, bool bOnAndroid)
{
	if (ConsoleCommandList.Num() == 0 || bForceParse)
	{
		ParseCommandList(bOnAndroid);
	}
	return ConsoleCommandList;
}

void UConsoleWidget::AddConsoleCommandsToIniFile(FString Category, TArray<FString> Commands, bool bReloadConfig)
{
#if PLATFORM_ANDROID
	extern FString GFilePathBase;
	FString ConfigFilePath = GFilePathBase + FString("/UE4Game/") + FApp::GetProjectName() + FString("/") + FApp::GetProjectName() + FString("/Plugins/VRDebugPlugin/Config/") + CommandListConfigName;
#else
	FString ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / CommandListConfigName;
#endif

	for (FString Command : Commands)
	{
		GConfig->SetString(*Category, *Command, TEXT(""), ConfigFilePath);
	}
	if (bReloadConfig)
	{
		GConfig->Exit();
		GConfig->InitializeConfigSystem();
	}
}

void UConsoleWidget::AddConsoleCommandToIniFile(FString Command, FString CommandCategory)
{
	FString ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / CommandListConfigName;

	if (GConfig)
	{
		GConfig->SetString(*CommandCategory, *Command, TEXT(""), ConfigFilePath);
		GConfig->Exit();
		GConfig->InitializeConfigSystem();
	}
	else
	{
		UE_LOG(VRDebugLog, Warning, TEXT("GConfig is not valid inside AddConsoleCommandToIniFile"));
	}
}	

void UConsoleWidget::RemoveConsoleCommandFromIniFile(FString Command, FString CommandCategory)
{
	FString ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / CommandListConfigName;

	if (GConfig)
	{
		GConfig->RemoveKey(*CommandCategory, *Command, *ConfigFilePath);
		GConfig->Exit();
		GConfig->InitializeConfigSystem();
	}
	else
	{
		UE_LOG(VRDebugLog, Warning, TEXT("GConfig is not valid inside RemoveConsoleCommandFromIniFile"));
	}
}

void UConsoleWidget::GetTextSize(UFont* Font, FString Text, float& Width, float& Height)
{
	FTextSizingParameters TextSizingParam;
	TextSizingParam.DrawFont = Font;
	TextSizingParam.Scaling = FVector2D(1.0, 1.0f);
	
	UCanvas::CanvasStringSize(TextSizingParam, *Text);

	Width = TextSizingParam.DrawXL;
	Height = TextSizingParam.DrawYL = Height;
}

void UConsoleWidget::ParseCommandList(bool bOnAndroid)
{
	ConsoleCommandList.Empty();

	//Create full config file path
	FString ConfigFilePath;
#if PLATFORM_ANDROID
	if (bOnAndroid)
	{
		extern FString GFilePathBase;
		ConfigFilePath = GFilePathBase + FString("/UE4Game/") + FApp::GetProjectName() + FString("/") + FApp::GetProjectName() + FString("/Plugins/VRDebugPlugin/Config/") + CommandListConfigName;
	}
	else
	{
		ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / CommandListConfigName;
	}
#else
	ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / CommandListConfigName;
#endif
	//Get all sections
	TArray<FString> Sections;
	GConfig->UnloadFile(ConfigFilePath);
	GConfig->Flush(true, ConfigFilePath);
	GConfig->GetSectionNames(ConfigFilePath, Sections);
	//Reset section index
	SectionIndex = -1;
	FGetValue GetValue(ConfigFilePath, ConsoleCommandList, SectionIndex);
	//Create value sink to read section entries
	FKeyValueSink ValueSink;
	ValueSink.BindRaw(&GetValue, &FGetValue::OnEntry);

	for (FString Section : Sections)
	{
		//Foreach section increase index, create new command list entry and loop through entries
		SectionIndex++;
		GetValue.SectionIndex = SectionIndex;
		FConsoleCommandList ConsoleCommand;
		ConsoleCommand.ListName = Section;
		ConsoleCommandList.Add(ConsoleCommand);
		GConfig->ForEachEntry(ValueSink, *Section, ConfigFilePath);
	}
}

void UConsoleWidget::OnEntry(const TCHAR* Key, const TCHAR* Value)
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
