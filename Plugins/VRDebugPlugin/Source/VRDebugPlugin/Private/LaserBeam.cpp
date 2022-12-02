// Copyright 2021 DownToCode. All Rights Reserved.

#include "LaserBeam.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "VRDebugComponent.h"
#include "VRDebugLog.h"

ALaserBeam::ALaserBeam()
	: Hand ( EControllerHand::Left )
	, WidgetInteractionDistance ( 1000 )
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	if (Root) RootComponent = Root;
	else { UE_LOG(VRDebugLog, Warning, TEXT("Could not create Root inside LaserBeam base class.")); }

	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComponent"));
	if (WidgetInteractionComponent)
	{
		WidgetInteractionComponent->SetupAttachment(RootComponent);
		WidgetInteractionComponent->InteractionDistance = WidgetInteractionDistance;
	}
	else { UE_LOG(VRDebugLog, Warning, TEXT("Could not create WidgetInteractionComponent inside LaserBeam base class.")); }

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	if (SplineComponent) SplineComponent->SetupAttachment(RootComponent);
	else { UE_LOG(VRDebugLog, Warning, TEXT("Could not create SplineComponent inside LaserBeam base class.")); }

	SplineMeshComponent = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SplineMeshComponent"));
	if (SplineComponent && SplineMeshComponent) SplineMeshComponent->SetupAttachment(SplineComponent);
	else { UE_LOG(VRDebugLog, Warning, TEXT("Could not create SplineMeshComponent or SplineComponent inside LaserBeam base class.")); }

}

void ALaserBeam::BeginPlay()
{
	Super::BeginPlay();
	EnableInput(GetWorld()->GetFirstPlayerController());
	if (VRDebugComponent->IsValidLowLevel() && !VRDebugComponent->bAutoSpawnBlueprintLog)
	{
		WidgetInteractionComponent->InteractionDistance = VRDebugComponent->SpawnDistanceFromCamera * 2;
		BindClick();
	}

	switch (Hand)
	{
	case EControllerHand::Left:
		WidgetInteractionComponent->PointerIndex = 0;
		WidgetInteractionComponent->VirtualUserIndex = 0;
		break;
	case EControllerHand::Right:
		WidgetInteractionComponent->PointerIndex = 1;
		WidgetInteractionComponent->VirtualUserIndex = 1;
		break;
	default:
		break;
	}
}

void ALaserBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALaserBeam::BindClick()
{
	if (ClickKey.IsValid() && InputComponent->IsValidLowLevel())
	{
		InputComponent->BindKey(ClickKey, EInputEvent::IE_Pressed, this, &ALaserBeam::InteractButtonPressed).bConsumeInput = false;
		InputComponent->BindKey(ClickKey, EInputEvent::IE_Released, this, &ALaserBeam::InteractButtonReleased).bConsumeInput = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Key isn't valid"));
	}
}

void ALaserBeam::UnbindClick()
{
	if (InputComponent->IsValidLowLevel())
	{
		InputComponent->KeyBindings.Empty();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Comp isn't Valid"));
	}
}
