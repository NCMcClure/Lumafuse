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