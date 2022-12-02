// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovableActor.generated.h"

class UVRDebugComponent;

UCLASS()
class VRDEBUGPLUGIN_API AMovableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovableActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Blueprint Log Actor", meta = (ExposeOnSpawn = true))
	UVRDebugComponent* VRDebugComponent;

	UFUNCTION(BlueprintImplementableEvent, Category = "Movable Actor")
	void StartHideActor(EControllerHand Hand);

	UFUNCTION(BlueprintImplementableEvent, Category = "Movable Actor")
	void ActorShown();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movable Actor")
	void FinishHideActor();
};
