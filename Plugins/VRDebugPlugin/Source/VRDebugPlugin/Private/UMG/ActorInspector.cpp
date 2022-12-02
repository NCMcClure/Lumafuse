// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/ActorInspector.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "VRDebugLog.h"


UActorInspector::UActorInspector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
      VariablesRefreshRate(.1f),
	  bInspecting (true)
{
	ScritpDelegate1.BindUFunction(this, "RunDelegate1");
	ScritpDelegate2.BindUFunction(this, "RunDelegate2");
	ScritpDelegate3.BindUFunction(this, "RunDelegate3");
	ScritpDelegate4.BindUFunction(this, "RunDelegate4");
	ScritpDelegate5.BindUFunction(this, "RunDelegate5");
	ScritpDelegate6.BindUFunction(this, "RunDelegate6");
	ScritpDelegate7.BindUFunction(this, "RunDelegate7");
	ScritpDelegate8.BindUFunction(this, "RunDelegate8");
	ScritpDelegate9.BindUFunction(this, "RunDelegate9");
	ScritpDelegate10.BindUFunction(this, "RunDelegate10");

	ScriptDelegates.Add(ScritpDelegate1);
	ScriptDelegates.Add(ScritpDelegate2);
	ScriptDelegates.Add(ScritpDelegate3);
	ScriptDelegates.Add(ScritpDelegate4);
	ScriptDelegates.Add(ScritpDelegate5);
	ScriptDelegates.Add(ScritpDelegate6);
	ScriptDelegates.Add(ScritpDelegate7);
	ScriptDelegates.Add(ScritpDelegate8);
	ScriptDelegates.Add(ScritpDelegate9);
	ScriptDelegates.Add(ScritpDelegate10);
}

void UActorInspector::OnActorRefreshVariables()
{
	if (!ActorReference)
	{
		UE_LOG(VRDebugLog, Warning, TEXT("Actor Reference is not valid inside %s"), *CUR_FUNC);
		return;
	}

	for (auto& Variable : ActorVariablesValues)
	{
		Variable.Value = UReflectionFL::GetActorVariableValueAsString(ActorReference, Variable.Key);
	}

	ActorVariablesTick();
}

TMap<FString, EPropType> UActorInspector::UpdateActor(AActor* Actor)
{
	if(!Actor)
	{
		UE_LOG(VRDebugLog, Error, TEXT("Actor reference not valid inside %s"), *CUR_FUNC);
		return TMap<FString, EPropType>();
	}

	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		UE_LOG(VRDebugLog, Error, TEXT("World reference not valid inside %s"), *CUR_FUNC);
		return TMap<FString, EPropType>();
	}

	World->GetTimerManager().ClearTimer(RefreshTimerHandle);

	
	TMap<FString, EPropType> ActorVariables = UReflectionFL::GetActorVariables(Actor);
	for (auto Variable : ActorVariables)
	{
		ActorVariablesValues.Add(Variable.Key, "N/A");
	}

	ActorReference = Actor;

	World->GetTimerManager().SetTimer(RefreshTimerHandle, this, &UActorInspector::OnActorRefreshVariables, VariablesRefreshRate, true);
	return ActorVariables;
}

TArray<FString> UActorInspector::GetActorDelegates(AActor* Actor)
{
	DelegateNames.Empty();
	for (int32 i = 0; i < DelegateNames.Num(); i++)
	{
		UReflectionFL::UnbindDelegate(Actor, DelegateNames[i], ScriptDelegates[i]);
	}

	DelegateNames = UReflectionFL::GetActorDelegates(Actor);

	for (int32 i = 0; i < ScriptDelegates.Num() ;i++)
	{
		if (!DelegateNames.IsValidIndex(i)) break;
		if (!ScriptDelegates.IsValidIndex(i)) break;

		UReflectionFL::BindToDelegate(Actor, DelegateNames[i], ScriptDelegates[i]);
	}

	return DelegateNames;
}

void UActorInspector::ResumeInspecting()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(VRDebugLog, Error, TEXT("World reference not valid inside %s"), *CUR_FUNC);
		return;
	}

	World->GetTimerManager().SetTimer(RefreshTimerHandle, this, &UActorInspector::OnActorRefreshVariables, VariablesRefreshRate, true);
	bInspecting = true;
}

void UActorInspector::StopInspecting()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(VRDebugLog, Error, TEXT("World reference not valid inside %s"), *CUR_FUNC);
		return;
	}

	World->GetTimerManager().ClearTimer(RefreshTimerHandle);
	bInspecting = false;
}

void UActorInspector::UpdateRefreshRate(float NewRefreshRate)
{
	StopInspecting();
	if (NewRefreshRate == 0) return;
	VariablesRefreshRate = NewRefreshRate;
	ResumeInspecting();
}

// ____________ACTOR DELEGATES____________//
/*
 * Hard coded delegates for catching blueprint made delegates callbacks
 * This is unfortunately the only way I know I can get a call back from a 
 * blueprint based delegate. The process is:
 * 
 * I create an FScriptDelegate that I need to bind to a function, then that delegete
 * gets added to a blueprint delegate call list that I discovered through reflection.
 * I cannot simply have a single function bound to a single FScriptDelegate as when
 * that function gets called I have no idea of which delegate called it. Instead if I 
 * keep a list of blueprint delegates name then based of the function called I can look up
 * the blueprint delegate and highlight it in the widget.
 *
 */
void UActorInspector::RunDelegate1()
{
	if (DelegateNames.IsValidIndex(0) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[0]);
	}
}

void UActorInspector::RunDelegate2()
{
	if (DelegateNames.IsValidIndex(1) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[1]);
	}
}

void UActorInspector::RunDelegate3()
{
	if (DelegateNames.IsValidIndex(2) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[2]);
	}
}

void UActorInspector::RunDelegate4()
{
	if (DelegateNames.IsValidIndex(3) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[3]);
	}
}

void UActorInspector::RunDelegate5()
{
	if (DelegateNames.IsValidIndex(4) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[4]);
	}
}

void UActorInspector::RunDelegate6()
{
	if (DelegateNames.IsValidIndex(5) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[5]);
	}
}

void UActorInspector::RunDelegate7()
{
	if (DelegateNames.IsValidIndex(6) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[6]);
	}
}

void UActorInspector::RunDelegate8()
{
	if (DelegateNames.IsValidIndex(7) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[7]);
	}
}

void UActorInspector::RunDelegate9()
{
	if (DelegateNames.IsValidIndex(8) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[8]);
	}
}

void UActorInspector::RunDelegate10()
{
	if (DelegateNames.IsValidIndex(9) && bInspecting)
	{
		OnDelegateRun.Broadcast(DelegateNames[9]);
	}
}

