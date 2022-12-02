// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UMG/DebugWidgetView.h"
#include "ReflectionFL.h"
#include "ActorInspector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDelegateRun, FString, DelegateName);

UCLASS()
class VRDEBUGPLUGIN_API UActorInspector : public UDebugWidgetView
{
	GENERATED_BODY()

public:
	UActorInspector(const FObjectInitializer& ObjectInitializer);

	void OnActorRefreshVariables();

	UFUNCTION(BlueprintImplementableEvent, Category = "Actor Inspector")
	void ActorVariablesTick();

	UFUNCTION(BlueprintCallable, Category = "Actor Inspector")
	TMap<FString, EPropType> UpdateActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Actor Inspector")
	TArray<FString> GetActorDelegates(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Actor Inspector")
	void ResumeInspecting();

	UFUNCTION(BlueprintCallable, Category = "Actor Inspector")
	void StopInspecting();

	UFUNCTION(BlueprintCallable, Category = "Actor Inspector")
	void UpdateRefreshRate(float NewRefreshRate);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Inspector")
	float VariablesRefreshRate;

	UPROPERTY(BlueprintReadWrite, Category = "Actor Inspector")
	TMap<FString, FString> ActorVariablesValues;

	UPROPERTY(BlueprintAssignable, Category = "Actor Inspector")
	FOnDelegateRun OnDelegateRun;

private:
	AActor* ActorReference;
	FTimerHandle RefreshTimerHandle;
	TArray<FString> DelegateNames;
	TArray<FScriptDelegate> ScriptDelegates;
	bool bInspecting;

	FScriptDelegate ScritpDelegate1;
	FScriptDelegate ScritpDelegate2;
	FScriptDelegate ScritpDelegate3;
	FScriptDelegate ScritpDelegate4;
	FScriptDelegate ScritpDelegate5;
	FScriptDelegate ScritpDelegate6;
	FScriptDelegate ScritpDelegate7;
	FScriptDelegate ScritpDelegate8;
	FScriptDelegate ScritpDelegate9;
	FScriptDelegate ScritpDelegate10;

	UFUNCTION()
	void RunDelegate1();
	UFUNCTION()
	void RunDelegate2();
	UFUNCTION()
	void RunDelegate3();
	UFUNCTION()
	void RunDelegate4();
	UFUNCTION()
	void RunDelegate5();
	UFUNCTION()
	void RunDelegate6();
	UFUNCTION()
	void RunDelegate7();
	UFUNCTION()
	void RunDelegate8();
	UFUNCTION()
	void RunDelegate9();
	UFUNCTION()
	void RunDelegate10();
};
