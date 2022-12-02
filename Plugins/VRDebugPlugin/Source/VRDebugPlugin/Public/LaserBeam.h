// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "LaserBeam.generated.h"

class UVRDebugComponent;

DECLARE_DYNAMIC_DELEGATE(FOnLaserBeamSpawned);

UENUM(BlueprintType)
enum class EPressType : uint8
{
	Pressed,
	Released
};

UCLASS()
class VRDEBUGPLUGIN_API ALaserBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserBeam();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Laser Beam")
	USceneComponent* Root;
	 
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Laser Beam")
	UWidgetInteractionComponent* WidgetInteractionComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Laser Beam")
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Laser Beam")
	USplineMeshComponent* SplineMeshComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Laser Beam")
	EControllerHand Hand;

	UPROPERTY(BlueprintReadOnly, Category = "Laser Beam")
	float WidgetInteractionDistance;

	UPROPERTY(BlueprintReadOnly, Category = "Laser Beam")
	UVRDebugComponent* VRDebugComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Laser Beam")
	FString FreeFlyingMode;

	UPROPERTY(BlueprintReadWrite, Category = "Laser Beam")
	FOnLaserBeamSpawned OnLaserBeamSpawned;

	UPROPERTY(BlueprintReadWrite, Category = "Laser Beam")
	AActor* VRDebugActor;

	UPROPERTY(BlueprintReadWrite, Category = "Laser Beam")
	bool bFreeFlyingEnabled = true;
	
	UPROPERTY()
	FKey ClickKey;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void InteractButtonPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void InteractButtonReleased();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Laser Beam")
	void StartReleasePointer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Laser Beam")
	void StopReleasePointer();

	UFUNCTION(BlueprintCallable, Category = "Laser Beam")
	void BindClick();

	UFUNCTION(BlueprintCallable, Category = "Laser Beam")
	void UnbindClick();

};

