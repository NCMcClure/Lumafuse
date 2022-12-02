// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

DECLARE_LOG_CATEGORY_EXTERN(VRDebugLog, Log, All);

#define CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))

#define LESS25 ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION < 25

#define GREATER24 (ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION > 24) || ENGINE_MAJOR_VERSION == 5

#define GREATER23 (ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION > 23) || ENGINE_MAJOR_VERSION == 5

#define GREATER22 (ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION > 22) || ENGINE_MAJOR_VERSION == 5

//Since in 4.25 UProperty has become FProperty if we typedef in here we avoid lots of 
//macro checks for the engine version we are running on
#if GREATER24
typedef FProperty UEProperty;
typedef FObjectProperty UEObjectProperty;
typedef FBoolProperty UEBoolProperty;
typedef FFloatProperty UEFloatProperty;
typedef FIntProperty UEIntProperty;
typedef FStrProperty UEStrProperty;
typedef FTextProperty UETextProperty;
typedef FNameProperty UENameProperty;
typedef FByteProperty UEByteProperty;
typedef FEnumProperty UEEnumProperty;
typedef FNumericProperty UENumericProperty;
typedef FMulticastDelegateProperty UEMulticastDelegateProperty;
#else
typedef UProperty UEProperty;
typedef UObjectProperty UEObjectProperty;
typedef UBoolProperty UEBoolProperty;
typedef UFloatProperty UEFloatProperty;
typedef UIntProperty UEIntProperty;
typedef UStrProperty UEStrProperty;
typedef UTextProperty UETextProperty;
typedef UNameProperty UENameProperty;
typedef UByteProperty UEByteProperty;
typedef UEnumProperty UEEnumProperty;
typedef UNumericProperty UENumericProperty;
typedef UMulticastDelegateProperty UEMulticastDelegateProperty;
#endif

//This makes it easier to use the right cast for 4.25 compatibility
template <typename To, typename From>
FORCEINLINE static To* UECast(From* Property)
{
#if GREATER24
	return CastField<To>(Property);
#else
	return Cast<To>(Property);
#endif
}
