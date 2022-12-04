// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OculusMovement/Public/OculusBodyTrackingComponent.h"
#include "LumafuseFunctionLibrary.generated.h"


/**
 * 
 */
UCLASS()
class LUMAFUSE_API ULumafuseFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Lumafuse|XR Body Tracking")
	static void UpdateArmsOutlineBoneTransforms(UOculusBodyTrackingComponent* oculusArms, UPoseableMeshComponent* outlineArms);
	
	UFUNCTION(BlueprintCallable, Category = "Lumafuse|Physics")
	static void SetAxisConstraintMode(UPrimitiveComponent* Primitive, bool LockPositionX, bool LockPositionY, bool LockPositionZ, bool LockRotationX, bool LockRotationY, bool LockRotationZ);

	UFUNCTION(BlueprintCallable, Category = "Lumafuse|Math")
	static float GetArraySumFloat(TArray<float> FloatArray);
	
	UFUNCTION(BlueprintCallable, Category = "Lumafuse|Math")
	static float GetMeanAverage(TArray<float> FloatArray);

	UFUNCTION(BlueprintCallable, Category = "Lumafuse|Math")
	static float GetVariance(TArray<float> FloatArray);	

	UFUNCTION(BlueprintCallable, Category = "Lumafuse|Math")
	static float GetStandardDeviation(TArray<float> FloatArray);

};
