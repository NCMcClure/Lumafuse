// Copyright 2021 DownToCode. All Rights Reserved.

#include "ReflectionFL.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Engine/Blueprint.h"
#include "Runtime/Engine/Classes/Engine/LevelScriptActor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/AssetManager.h"

UClass* UReflectionFL::SkySphereClass;

UReflectionFL::UReflectionFL()
{
#if WITH_EDITOR
	ConstructorHelpers::FObjectFinder<UBlueprint> SkySphere(TEXT("/Engine/EngineSky/BP_Sky_Sphere.BP_Sky_Sphere"));
	SkySphereClass = nullptr;
	if (SkySphere.Succeeded())
	{
		SkySphereClass = (UClass*)SkySphere.Object->GeneratedClass;
	}
#endif	
}

void UReflectionFL::GetAllActorsInWorld(UObject* Caller, TArray<AActor*>& Actors, TArray<FString>& ActorNames, TArray<FString>& ActorClassNames)
{
	if (Caller)
	{
		if (Caller->GetWorld())
		{
			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(Caller, AActor::StaticClass(), OutActors);

			ALevelScriptActor* LevelScriptActor = Caller->GetWorld()->GetLevelScriptActor();
			ActorNames.Add(TEXT("Level Blueprint"));
			ActorClassNames.Add(ALevelScriptActor::StaticClass()->GetName());
			Actors.Add(LevelScriptActor);

			for (AActor* Actor : OutActors)
			{
				UClass* ActorClass = Actor->GetClass();
		if (!ActorClass->IsNative() && GetActorFunctionsEvents(Actor).Num() > 0)
				{
					if (!IsActorFromProjectOrLevelBlueprint(Caller, ActorClass)) continue;

					Actors.Add(Actor);
					FString ClassName = Actor->GetClass()->GetName();
					FString ActorName = Actor->GetName();
					int32 index = ClassName.Find("_C");
					if (index != -1) 
					{
						ClassName = ClassName.Left(index);
					}
					else 
					{
						ClassName.FindLastChar('_', index);
						ClassName = ClassName.Left(index);
					}
					ActorNames.Add(ActorName);
					ActorClassNames.Add(ClassName);
				}
			}
		}
	}
	else
	{
		UE_LOG(VRDebugLog, Error, TEXT("UObject ins't valid insidew GetAllActorsInWorld"));
	}
}

TMap<FString, EPropType> UReflectionFL::GetActorVariables(AActor* Actor)
{
	UClass* Class = nullptr;
	if (!IsValidObject(Actor, Class)) return TMap<FString, EPropType>();

	TMap<FString, EPropType> Variables;

	for (TFieldIterator<UEProperty> it(Class, EFieldIteratorFlags::ExcludeSuper); it; ++it)
	{
		UEProperty* Property = *it;
		EPropType PropertyType = EPropType::Bool;
		if (Property && IsSupportedActorProperty(Property, PropertyType))
		{
			if (PropertyType == EPropType::Object && GetPropertyObjectClass(Actor, Property) && !(GetPropertyObjectClass(Actor, Property)->IsA<USceneComponent>()))
			{
				Variables.Add(Property->GetName(), PropertyType);
			}
			else if(!(PropertyType == EPropType::Object))
			{
				Variables.Add(Property->GetName(), PropertyType);
			}

		}
	}

	return Variables;
}

FString UReflectionFL::GetActorVariableValueAsString(AActor* Actor, FString VariableName)
{
	UClass* Class = nullptr;
	if (!IsValidObject(Actor, Class)) return FString();
	
	for (TFieldIterator<UEProperty> it(Class, EFieldIteratorFlags::ExcludeSuper); it; ++it)
	{
		UEProperty* Property = *it;
		EPropType PropertyType = EPropType::Bool;
		if (Property && Property->GetName() == VariableName && IsSupportedActorProperty(Property, PropertyType))
		{
			return GetPropertyValueAsString(Actor, Property, PropertyType);
		}
	}

	return FString("Not Valid");
}

TArray<FString> UReflectionFL::GetActorFunctionsEvents(AActor* Actor)
{
	UClass* Class = nullptr;
	if (!IsValidObject(Actor, Class)) return TArray<FString>();

	TArray<FString> FunctionNames;

	for (TFieldIterator<UFunction> it(Class, EFieldIteratorFlags::ExcludeSuper); it; ++it)
	{
		UFunction* Func = *it;
		bool bHasSupportedFields = true;

		for (TFieldIterator<UEProperty> PropIt(Func, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
		{
			UEProperty* Property = *PropIt;
			if (Property->HasAnyPropertyFlags(CPF_Parm))
			{				
				if (!Property->HasAnyPropertyFlags(CPF_OutParm) && Property->GetName() != "EntryPoint")
				{
					EPropType Type;
					if (!IsSupportedProperty(Property, Type))
					{
						bHasSupportedFields = false;
					}
				}
				
			}
		}
		
		if (Class->IsFunctionImplementedInScript(FName(*Func->GetName())) &&
			(Func->HasAnyFunctionFlags(EFunctionFlags::FUNC_BlueprintEvent) && (!Func->HasAnyFunctionFlags(EFunctionFlags::FUNC_Event))) &&
			!Func->GetName().EndsWith("_UpdateFunc") &&
			!Func->GetName().EndsWith("_FinishedFunc") &&
			!Func->GetName().EndsWith("_DelegateSignature") &&
			bHasSupportedFields)
		{
			FunctionNames.Add(Func->GetName());
		}
	}

	return FunctionNames;
}

TMap<FString, EPropType> UReflectionFL::GetFunctionParameters(AActor* Actor, FString FunctionName)
{
	if(!IsValidObject(Actor)) return TMap<FString, EPropType>();

	UFunction* Function = Actor->FindFunction(FName(*FunctionName));

	if (!Function)
	{
		UE_LOG(VRDebugLog, Error, TEXT("Function isn't valid inside of GetFunctionParamters"));
		return TMap<FString, EPropType>();
	}

	TMap<FString, EPropType> Params;

	for (TFieldIterator<UEProperty> PropIt(Function, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
	{
		UEProperty* Property = *PropIt;
		if (Property->HasAnyPropertyFlags(CPF_Parm))
		{
			if (!Property->HasAnyPropertyFlags(CPF_OutParm) && Property->GetName() != "EntryPoint")
			{
				EPropType Type;
				if (IsSupportedProperty(Property, Type))
				{
					Params.Add(Property->GetName(), Type);
				}
			}
		}
	}

	return Params;
}

TArray<FString> UReflectionFL::GetLevelBlueprintFunctions(UObject* Caller)
{
	TArray<FString> Functions;
	if (!IsValidObject(Caller)) return Functions;

	UWorld* World = Caller->GetWorld();

	if (!World) 
	{
		UE_LOG(VRDebugLog, Error, TEXT("World ins't valid insidew GetLevelBlueprintFunctions"));
		return Functions;
	}

	ALevelScriptActor* LevelScriptActor = World->GetLevelScriptActor();
	if (!LevelScriptActor)
	{
		UE_LOG(VRDebugLog, Error, TEXT("LevelScriptActor ins't valid insidew GetLevelBlueprintFunctions"));
		return Functions;
	}

	Functions = GetActorFunctionsEvents(LevelScriptActor);

	return Functions;
}

TMap<FString, EPropType> UReflectionFL::GetLevelBlueprintFunctionParameters(UObject* Caller, FString FunctionName)
{
	TMap<FString, EPropType> Parameters;
	if (!IsValidObject(Caller)) return Parameters;

	UWorld* World = Caller->GetWorld();

	if (!World)
	{
		UE_LOG(VRDebugLog, Error, TEXT("World ins't valid insidew GetLevelBlueprintFunctions"));
		return Parameters;
	}

	ALevelScriptActor* LevelScriptActor = World->GetLevelScriptActor();
	if (!LevelScriptActor)
	{
		UE_LOG(VRDebugLog, Error, TEXT("LevelScriptActor ins't valid insidew GetLevelBlueprintFunctions"));
		return Parameters;
	}

	Parameters = GetFunctionParameters(LevelScriptActor, FunctionName);

	return Parameters;
}

bool UReflectionFL::CallFunctionByName(AActor* Actor, FString FunctionName)
{
	if (!IsValidObject(Actor)) return false;

	return Actor->CallFunctionByNameWithArguments(*FunctionName, *GLog, NULL, true);
}

bool UReflectionFL::CallFunctionByNameWithParams(AActor* Actor, FString FunctionName, TArray<FString> Params)
{
	if (!IsValidObject(Actor)) return false;
	
	FString RunCommand = FunctionName;

	for (FString Param : Params)
	{
		RunCommand += " ";
		RunCommand += Param;
	}

	return Actor->CallFunctionByNameWithArguments(*RunCommand, *GLog, NULL, true);
}

void UReflectionFL::BindToFunctionByName(AActor* Actor, FName Function)
{
	if (!IsValidObject(Actor)) return;

	UFunction* Func = Actor->FindFunction(Function);
	
	if (Func)
	{	
		for (UField* Property = Func->Children; Property; Property = Property->Next)
		{
			UE_LOG(VRDebugLog, Error, TEXT("Property: %s"), *Property->GetName());
		}
	}
	else
	{
		UE_LOG(VRDebugLog, Error, TEXT("Could not find function inside BindToFunctionByName"));
		return;
	}
}

TArray<FString> UReflectionFL::GetActorDelegates(AActor* Actor)
{
	TArray<FString> DelegateNames;

	if (!IsValidObject(Actor)) return DelegateNames;

	for (TFieldIterator<UEMulticastDelegateProperty> PropertyIt(Actor->GetClass(), EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		UEMulticastDelegateProperty* DelegateProperty = UECast<UEMulticastDelegateProperty>(*PropertyIt);

		if (DelegateProperty && !DelegateProperty->IsNative())
		{
			const FMulticastScriptDelegate* Delegate = DelegateProperty->GetMulticastDelegate((*PropertyIt)->ContainerPtrToValuePtr<FMulticastScriptDelegate>(Actor));

			if (Delegate)
			{
				DelegateNames.Add(*PropertyIt->GetName());
			}
		}
	}

	return DelegateNames;
}

void UReflectionFL::BindToDelegate(AActor* Actor, FString DelegateName, FScriptDelegate ScriptDelegate)
{
	if (!IsValidObject(Actor)) return;
	
	for (TFieldIterator<UEMulticastDelegateProperty> PropertyIt(Actor->GetClass(), EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		UEMulticastDelegateProperty* DelegateProperty = UECast<UEMulticastDelegateProperty>(*PropertyIt);

		if (DelegateProperty && !DelegateProperty->IsNative())
		{
			const FMulticastScriptDelegate* Delegate = DelegateProperty->GetMulticastDelegate((*PropertyIt)->ContainerPtrToValuePtr<FMulticastScriptDelegate>(Actor));

			if (Delegate && *PropertyIt->GetName() == DelegateName)
			{
				UFunction* Func = DelegateProperty->SignatureFunction;
				//@todo: read properties of the function signature so that we can report the parameter this delegate has passed with it.
				//for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It){} CallFuynctionHandler.cpp line 143
				DelegateProperty->RemoveDelegate(ScriptDelegate, Actor, (*PropertyIt)->ContainerPtrToValuePtr<FMulticastScriptDelegate>(Actor));
				DelegateProperty->AddDelegate(ScriptDelegate, Actor, (*PropertyIt)->ContainerPtrToValuePtr<FMulticastScriptDelegate>(Actor));
			}
		}
	}
}

void UReflectionFL::UnbindDelegate(AActor* Actor, FString DelegateName, FScriptDelegate ScriptDelegate)
{
	if (!IsValidObject(Actor)) return;

	for (TFieldIterator<UEMulticastDelegateProperty> PropertyIt(Actor->GetClass(), EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		UEMulticastDelegateProperty* DelegateProperty = UECast<UEMulticastDelegateProperty>(*PropertyIt);

		if (DelegateProperty && !DelegateProperty->IsNative())
		{
			const FMulticastScriptDelegate* Delegate = DelegateProperty->GetMulticastDelegate((*PropertyIt)->ContainerPtrToValuePtr<FMulticastScriptDelegate>(Actor));

			if (Delegate && *PropertyIt->GetName() == DelegateName)
			{
				UFunction* Func = DelegateProperty->SignatureFunction;
				//@todo: read properties of the function signature so that we can report the parameter this delegate has passed with it.
				//for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It){} CallFuynctionHandler.cpp line 143
				DelegateProperty->RemoveDelegate(ScriptDelegate, Actor, (*PropertyIt)->ContainerPtrToValuePtr<FMulticastScriptDelegate>(Actor));
			}
		}
	}
}

DEFINE_FUNCTION(UReflectionFL::FunctionRunCallback)
{
	UE_LOG(VRDebugLog, Error, TEXT("FUNCTION RUN!!!"));
}

bool UReflectionFL::IsSupportedProperty(UEProperty* Property, EPropType& Type)
{
	if (UECast<UEBoolProperty>(Property))
	{
		Type = EPropType::Bool;
		return true;
	}

	if (UECast<UEFloatProperty>(Property))
	{
		Type = EPropType::Float;
		return true;
	}

	if (UECast<UEIntProperty>(Property))
	{
		Type = EPropType::Int;
		return true;
	}

	if (UECast<UEStrProperty>(Property))
	{
		Type = EPropType::String;
		return true;
	}

	if (UECast<UETextProperty>(Property))
	{
		Type = EPropType::Text;
		return true;
	}

	if (UECast<UENameProperty>(Property))
	{
		Type = EPropType::Name;
		return true;
	}

	return false;
}

bool UReflectionFL::IsSupportedActorProperty(UEProperty* Property, EPropType& Type)
{
	if (IsSupportedProperty(Property, Type))
		return true;

	if (UECast<UEByteProperty>(Property))
	{
		Type = EPropType::BPEnum;
		return true;
	}

	if (UECast<UEEnumProperty>(Property))
	{
		Type = EPropType::CPPEnum;
		return true;
	}

	if (UECast<UEObjectProperty>(Property))
	{
		Type = EPropType::Object;
		return true;
	}

	return false;
}

bool UReflectionFL::IsActorFromProjectOrLevelBlueprint(UObject* Caller, UClass* ActorClass)
{
	FString ClassName = ActorClass->GetName().ToLower();

#if WITH_EDITOR
	if (SkySphereClass && ActorClass->IsChildOf(SkySphereClass)) return false;
#else
	if (ClassName.Contains("sky") || ClassName.Contains("sphere") || ClassName.Contains("skysphere")) return false;
#endif

	TArray<FAssetData> Assets;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegestry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add("/VRDebugPlugin");

	AssetRegestry.GetAssets(Filter, Assets);
	for (FAssetData Asset : Assets)
	{
		if (ClassName.Contains(Asset.AssetName.ToString())) return false;
	}

	ALevelScriptActor* LevelScriptActor = Caller->GetWorld()->GetLevelScriptActor();
	if (LevelScriptActor)
	{
		if(ActorClass->IsChildOf(LevelScriptActor->StaticClass())) return false;
	}

	return true;
}

bool UReflectionFL::IsValidObject(UObject* Object)
{
	if (!Object)
	{
		UE_LOG(VRDebugLog, Error, TEXT("Actor isn't valid inside %s"), *CUR_FUNC);
		return false;
	}

	return true;
}

bool UReflectionFL::IsValidObject(UObject* Object, UClass*& Class)
{
	if (!Object)
	{
		UE_LOG(VRDebugLog, Error, TEXT("Actor isn't valid inside %s"), *CUR_FUNC);
		return false;
	}

	Class = Object->GetClass();
	if (!Class)
	{
		UE_LOG(VRDebugLog, Error, TEXT("Actor's UClass isn't valid inside %s"), *CUR_FUNC);
		return false;
	}

	return true;
}

FString UReflectionFL::GetPropertyValueAsString(AActor* Actor, UEProperty* Property, EPropType PropertyType)
{
	switch (PropertyType)
	{
	case EPropType::Bool:
		{
			bool BoolValue = UECast<UEBoolProperty>(Property)->GetPropertyValue(Property->ContainerPtrToValuePtr<UEBoolProperty>(Actor));
			if (BoolValue) return FString("True");
			else return FString("False");
			break;
		}

	case EPropType::Float:
		{
			float FloatValue = UECast<UEFloatProperty>(Property)->GetPropertyValue(Property->ContainerPtrToValuePtr<UEFloatProperty>(Actor));
			return FString::SanitizeFloat(FloatValue);
			break;
		}

	case EPropType::Int:
		{
			int32 IntValue = UECast<UEIntProperty>(Property)->GetPropertyValue(Property->ContainerPtrToValuePtr<UEIntProperty>(Actor));
			return FString::FromInt(IntValue);
			break;
		}

	case EPropType::Name:
		{
			FName NameValue = UECast<UENameProperty>(Property)->GetPropertyValue(Property->ContainerPtrToValuePtr<UENameProperty>(Actor));
			return NameValue.ToString();
			break; 
		}

	case EPropType::String:
		{
			FString StringValue = UECast<UEStrProperty>(Property)->GetPropertyValue(Property->ContainerPtrToValuePtr<UEStrProperty>(Actor));
			return StringValue;
			break;
		}

	case EPropType::Text:
		{
			FText TextValue = UECast<UETextProperty>(Property)->GetPropertyValue(Property->ContainerPtrToValuePtr<UETextProperty>(Actor));
			return TextValue.ToString();
			break;
		}

	case EPropType::Object:
		{
			UObject* ObjectValue = UECast<UEObjectProperty>(Property)->GetPropertyValue(Property->ContainerPtrToValuePtr<UEObjectProperty>(Actor));
			if (ObjectValue) return ObjectValue->GetName();
			else return FString("Not Valid");
			break;
		}

	case EPropType::CPPEnum:
		{
			UEEnumProperty* EnumInternalProp = UECast<UEEnumProperty>(Property);
			UENumericProperty* EnumNumericProp = EnumInternalProp->GetUnderlyingProperty();
			const int64 EnumValue = EnumNumericProp->GetSignedIntPropertyValue(Property->ContainerPtrToValuePtr<UENumericProperty>(Actor));
			return EnumInternalProp->GetEnum()->GetDisplayNameTextByValue(EnumValue).ToString();
			break;
		}

	case EPropType::BPEnum:
		{
			UEByteProperty* EnumProperty = UECast<UEByteProperty>(Property);
			uint8 EnumValue = EnumProperty->GetPropertyValue(Property->ContainerPtrToValuePtr<uint8>(Actor));
			return EnumProperty->Enum->GetDisplayNameTextByValue(EnumValue).ToString();
			break;
		}

	default:
		return FString();
		break;

	}
}

UObject* UReflectionFL::GetPropertyObjectClass(AActor* Actor, UEProperty* Property)
{
	if (UECast<UEObjectProperty>(Property))
	{
		UObject* ObjectValue = UECast<UEObjectProperty>(Property)->GetPropertyValue(Property->ContainerPtrToValuePtr<UEObjectProperty>(Actor));
		if (ObjectValue)
		{
			return ObjectValue;
		}
	}
	
	return nullptr;
}
