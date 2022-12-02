// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VRDebugLog.h"
#include "ReflectionFL.generated.h"

UENUM(BlueprintType)
enum class EPropType : uint8
{
	Bool,
	Float,
	Int,
	String,
	Name,
	Text,
	Object,
	CPPEnum,
	BPEnum,
	Delegate
};

UCLASS()
class VRDEBUGPLUGIN_API UReflectionFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UReflectionFL();

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library", meta = (DefaultToSelf="Caller", HidePin="Caller"))
	static void GetAllActorsInWorld(UObject* Caller, TArray<AActor*>& Actors, TArray<FString>& ActorNames, TArray<FString>& ActorClassNames);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library")
	static TMap<FString, EPropType> GetActorVariables(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library")
	static FString GetActorVariableValueAsString(AActor* Actor, FString VariableName);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library")
	static TArray<FString> GetActorFunctionsEvents(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library")
	static TMap<FString, EPropType> GetFunctionParameters(AActor* Actor, FString FunctionName);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library", meta = (DefaultToSelf = "Caller", HidePin = "Caller"))
	static TArray<FString> GetLevelBlueprintFunctions(UObject* Caller);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library", meta = (DefaultToSelf = "Caller", HidePin = "Caller"))
	static TMap<FString, EPropType> GetLevelBlueprintFunctionParameters(UObject* Caller, FString FunctionName);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library")
	static bool CallFunctionByName(AActor* Actor, FString FunctionName);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library")
	static bool CallFunctionByNameWithParams(AActor* Actor, FString FunctionName, TArray<FString> Params);

	UFUNCTION(BlueprintCallable, Category = "VR Debug Plugin | Reflection Library")
	static TArray<FString> GetActorDelegates(AActor* Actor);

	static void BindToDelegate(AActor* Actor, FString DelegateName, FScriptDelegate ScriptDelegate);

	static void UnbindDelegate(AActor* Actor, FString DelegateName, FScriptDelegate ScriptDelegate);

#pragma region Experimental
	static void BindToFunctionByName(AActor* Actor, FName Function);

	DECLARE_FUNCTION(FunctionRunCallback);
#pragma endregion

private:

	static bool IsSupportedProperty(UEProperty* Property, EPropType& Type);

	static bool IsSupportedActorProperty(UEProperty* Property, EPropType& Type);

	static bool IsActorFromProjectOrLevelBlueprint(UObject* Caller, UClass* ActorClass);

	static bool IsValidObject(UObject* Object);

	static bool IsValidObject(UObject* Object, UClass*& Class);

	static FString GetPropertyValueAsString(AActor* Actor, UEProperty* Property, EPropType PropertyType);

	static UObject* GetPropertyObjectClass(AActor* Actor, UEProperty* Property);

	static UClass* SkySphereClass;

};
