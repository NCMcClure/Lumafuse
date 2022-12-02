// Copyright 2021 DownToCode. All Rights Reserved.

#include "BlueprintLogBase.h"

ABlueprintLogBase::ABlueprintLogBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABlueprintLogBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABlueprintLogBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

