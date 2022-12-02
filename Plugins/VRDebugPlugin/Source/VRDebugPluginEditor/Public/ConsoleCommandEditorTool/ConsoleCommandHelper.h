// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "SVRDebugSuggestionTextBox.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "Runtime/EngineSettings//Classes/ConsoleSettings.h"
#include "Runtime/Core/Public/HAL/IConsoleManager.h"
#include "Runtime/Engine/Public/ShowFlags.h"
#include "TreeItem.h"

class FVRDebugConsoleVariableAutoCompleteVisitor
{
public:
	// @param Name must not be 0
	// @param CVar must not be 0
	static void OnConsoleVariable(const TCHAR* Name, IConsoleObject* CVar, TArray<struct FAutoCompleteCommand>& Sink)
	{
#if DISABLE_CHEAT_CVARS
		if (CVar->TestFlags(ECVF_Cheat))
		{
			return;
		}
#endif // DISABLE_CHEAT_CVARS
		if (CVar->TestFlags(ECVF_Unregistered))
		{
			return;
		}

		const UConsoleSettings* ConsoleSettings = GetDefault<UConsoleSettings>();

		// can be optimized
		int32 NewIdx = Sink.AddDefaulted();
		FAutoCompleteCommand Cmd;
		Cmd.Command = Name;
		Sink.Add(Cmd);
	}
};

class FGetConsoleValue
{
public:
	const FString& IniFilename;
	FString SectionName;
	TMap<FString,TArray<FString>>& ConsoleCommandList;

	FGetConsoleValue(const FString& InIniFilename, TMap<FString,TArray<FString>>& InConsoleCommandList, FString InSectionName)
		: IniFilename(InIniFilename)
		, SectionName(InSectionName)
		, ConsoleCommandList(InConsoleCommandList)
	{}

	void OnEntry(const TCHAR* Key, const TCHAR* Value)
	{
		if (Key)
		{
			ConsoleCommandList.FindOrAdd(SectionName).Add(Key);
		}
		else return;
	}
};

class FConsoleCommandHelper
{

public:

	static void GetConsoleCommandsIniFile(TMap<FString, TArray<FString>>& OutConsoleCommands)
	{
		OutConsoleCommands.Empty();
		FString ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / TEXT("Commands.ini");
		//Get all sections
		TArray<FString> Sections;
		GConfig->UnloadFile(ConfigFilePath);
		GConfig->Flush(true, ConfigFilePath);
		GConfig->GetSectionNames(ConfigFilePath, Sections);
		//Reset section index
		FString SectionName;
		FGetConsoleValue GetValue(ConfigFilePath, OutConsoleCommands, SectionName);
		//Create value sink to read section entries
		FKeyValueSink ValueSink;
		ValueSink.BindRaw(&GetValue, &FGetConsoleValue::OnEntry);

		for (int32 i = 0; i < Sections.Num(); i++)
		{
			//Foreach section increase index, create new command list entry and loop through entries
			GetValue.SectionName = Sections[i];
			GConfig->ForEachEntry(ValueSink, *Sections[i], ConfigFilePath);
		}
	}

	static void SaveConsoleCommandsIniFile(TMap<FString, TArray<FString>> ConsoleCommands)
	{
		FString ConfigFilePath = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / TEXT("Commands.ini");

		TArray<FString> Sections;
		GConfig->GetSectionNames(ConfigFilePath, Sections);
		for (int32 i = 0; i < Sections.Num(); i++)
		{
			GConfig->EmptySection(*Sections[i], ConfigFilePath);
		}
		GConfig->Flush(false, ConfigFilePath);

		if (GConfig)
		{
			TArray<FString> Categories;
			ConsoleCommands.GetKeys(Categories);

			bool bFoundCustom = false;
			for (auto Category : Categories) { if (Category == "Custom") bFoundCustom = true; }
			if (!bFoundCustom) GConfig->SetString(*FString("Custom"), TEXT("DONOTDELETE"), TEXT(""), ConfigFilePath);

			for (int32 i = Categories.Num() - 1; i >=0; i--)
			{
				if (Categories[i] == "Custom" && (*ConsoleCommands.Find(Categories[i])).Num() == 0)
				{
					bFoundCustom = true;
					GConfig->SetString(*Categories[i], TEXT("DONOTDELETE"), TEXT(""), ConfigFilePath);
				}
				else
				{
					if (Categories[i] == "Custom")
					{
						bFoundCustom = true;
						GConfig->SetString(*Categories[i], TEXT("DONOTDELETE"), TEXT(""), ConfigFilePath);
					}

					for (FString Value : *ConsoleCommands.Find(Categories[i]))
					{
						GConfig->SetString(*Categories[i], *Value, TEXT(""), ConfigFilePath);
					}
				}
			}

			GConfig->Flush(false, ConfigFilePath);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GConfig isn't valid inside SaveConsoleCommandsIniFile"));
		}

	}

	static void GetEditorConsoleCommands(TArray<FString>& OutEditorConsoleCommands)
	{
		const UConsoleSettings* ConsoleSettings = GetDefault<UConsoleSettings>();
		if (ConsoleSettings)
		{
			for (int32 i = 0; i < ConsoleSettings->ManualAutoCompleteList.Num(); i++)
			{
				OutEditorConsoleCommands.Add(ConsoleSettings->ManualAutoCompleteList[i].Command);
			}
		}

		// Add all showflag commands.
		{
			struct FIterSink
			{
				FIterSink(TArray<FString>& InAutoCompleteList)
					: AutoCompleteList(InAutoCompleteList)
				{
				}

				bool OnEngineShowFlag(uint32 InIndex, const FString& InName)
				{
					// Get localized name.
					FText LocName;
					FEngineShowFlags::FindShowFlagDisplayName(InName, LocName);
					AutoCompleteList.Add(TEXT("show ") + InName);
					return true;
				}

				bool OnCustomShowFlag(uint32 InIndex, const FString& InName)
				{
					// Get localized name.
					FText LocName;
					FEngineShowFlags::FindShowFlagDisplayName(InName, LocName);
					AutoCompleteList.Add(TEXT("show ") + InName);
					return true;
				}

				TArray<FString>& AutoCompleteList;
			};
			TArray<FString> OutCommands;
			FIterSink Sink(OutCommands);
			for (int32 i = 0; i < OutCommands.Num(); i++)
			{
				OutEditorConsoleCommands.Add(OutCommands[i]);
			}

			FEngineShowFlags::IterateAllFlags(Sink);
		}


		TArray<FAutoCompleteCommand> NewCommands;
		IConsoleManager::Get().ForEachConsoleObjectThatStartsWith(
			FConsoleObjectVisitor::CreateStatic< TArray<struct FAutoCompleteCommand>& >(
				&FVRDebugConsoleVariableAutoCompleteVisitor::OnConsoleVariable,
				NewCommands));

		for (int32 i = 0; i < NewCommands.Num(); i++)
		{
			OutEditorConsoleCommands.Add(NewCommands[i].Command);
		}
	}
};
