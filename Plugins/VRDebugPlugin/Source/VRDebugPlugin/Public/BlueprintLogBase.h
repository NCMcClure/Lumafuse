// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlueprintLogBase.generated.h"

class UVRDebugComponent;

UCLASS()
class VRDEBUGPLUGIN_API ABlueprintLogBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlueprintLogBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Blueprint Log Actor")
	UVRDebugComponent* VRDebugComponent;
};
