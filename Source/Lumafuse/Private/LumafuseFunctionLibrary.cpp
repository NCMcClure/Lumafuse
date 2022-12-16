// Fill out your copyright notice in the Description page of Project Settings.


#include "LumafuseFunctionLibrary.h"

void ULumafuseFunctionLibrary::UpdateArmsOutlineBoneTransforms(UOculusBodyTrackingComponent* oculusArms,
	UPoseableMeshComponent* outlineArms)
{
	TArray<FName> boneNames;
	oculusArms->BoneNames.GenerateValueArray(boneNames);

	for (int boneIndex = 0; boneIndex < boneNames.Num(); boneIndex++)
	{
		FTransform tempBoneTransform = oculusArms->GetBoneTransformByName(boneNames[boneIndex], EBoneSpaces::WorldSpace);

		outlineArms->SetBoneTransformByName(boneNames[boneIndex], tempBoneTransform, EBoneSpaces::WorldSpace);
	}
	
}

void ULumafuseFunctionLibrary::SetAxisConstraintMode(UPrimitiveComponent* Primitive, const bool LockPositionX,
	const bool LockPositionY, const bool LockPositionZ, const bool LockRotationX, const bool LockRotationY, const bool LockRotationZ)
{

	Primitive->GetBodyInstance()->bLockXTranslation = LockPositionX;
	Primitive->GetBodyInstance()->bLockYTranslation = LockPositionY;
	Primitive->GetBodyInstance()->bLockZTranslation = LockPositionZ;
	Primitive->GetBodyInstance()->bLockXRotation = LockRotationX;
	Primitive->GetBodyInstance()->bLockYRotation = LockRotationY;
	Primitive->GetBodyInstance()->bLockZRotation = LockRotationZ;
	
}

float ULumafuseFunctionLibrary::GetArraySumFloat(TArray<float> FloatArray)
{
	float TempFloat = 0;

	for (auto TargetFloat : FloatArray)
	{
		TempFloat = TempFloat + TargetFloat;
	}

	return TempFloat;
}

float ULumafuseFunctionLibrary::GetMeanAverage(TArray<float> FloatArray)
{
	float TempFloat = 0;

	for (auto TargetFloat : FloatArray)
	{
		TempFloat = TempFloat + TargetFloat; 
	}

	TempFloat = TempFloat / FloatArray.Num();

	return TempFloat;
}

float ULumafuseFunctionLibrary::GetVariance(TArray<float> FloatArray)
{
	float TempFloat = 0;
	float const Average = GetMeanAverage(FloatArray);
	TArray<float> SumOfDifferences;

	for (auto TargetFloat : FloatArray)
	{
		SumOfDifferences.Add(TargetFloat - Average);
	}

	TArray<float> SquaredDifferences;

	for (auto TargetFloat : SumOfDifferences)
	{
		SquaredDifferences.Add(pow(TargetFloat, 2));
	}

	TempFloat = GetArraySumFloat(SquaredDifferences);
	TempFloat = TempFloat / (SquaredDifferences.Num() - 1);

	return  TempFloat;
}

float ULumafuseFunctionLibrary::GetStandardDeviation(TArray<float> FloatArray)
{
	float TempFloat = 0;
	TempFloat = sqrt(GetVariance(FloatArray));
	return TempFloat;
}

//Write a function that takes in a float array and returns the median value
float ULumafuseFunctionLibrary::GetMedian(TArray<float> FloatArray)
{
	float TempFloat = 0;
	FloatArray.Sort();
	if (FloatArray.Num() % 2 == 0)
	{
		TempFloat = (FloatArray[FloatArray.Num() / 2] + FloatArray[(FloatArray.Num() / 2) - 1]) / 2;
	}
	else
	{
		TempFloat = FloatArray[FloatArray.Num() / 2];
	}
	return TempFloat;
}

