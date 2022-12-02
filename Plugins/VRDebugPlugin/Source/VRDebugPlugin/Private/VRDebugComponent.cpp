// Copyright 2021 DownToCode. All Rights Reserved.

#include "VRDebugComponent.h"

#include "DrawDebugHelpers.h"
#include "VRDebugLog.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#if WITH_EDITOR
#include "Editor/UnrealEd/Public/Editor.h"
#endif

TArray<FName> UVRDebugComponent::MotionControllerCandidates;

UVRDebugComponent::UVRDebugComponent()
	: bShouldBindInputs(true)
	, OpenMenuKey(EKeys::OculusTouch_Right_A_Click)
	, WidgetInteractionChannel(ECollisionChannel::ECC_Visibility)
	, VRWidgetScale(0.1f)
	, SpawnDistanceFromCamera(100.0f)
	, LeftHandWidgetClickKey(EKeys::OculusTouch_Left_Trigger_Click)
	, RightHandWidgetClickKey(EKeys::OculusTouch_Right_Trigger_Click)
	, bUseRelativePath(false)
	, bTryCreateFolder(false)
	, CountDown(3)
	, CameraFieldOfView(90)
	, ScreenshotResolutionX(1024)
	, ScreenshotResolutionY(1024)
	, ConsoleCategoryColumnWidth(650.0f)
	, WidgetFontSize(20.0f)
	, OutputLogMaxMessages(10000)
	, BlueprintLogMaxMessages(50)
	, BlueprintLogSpawnDistanceFromCamera(100.0f)
	, BlueprintLogMinWidth (600.0f)
	, BlueprintLogMaxWidth (900.0f)
	, bAutoSpawnBlueprintLog(false)
	, bBlueprintLogDrawsOnTop(false)
	, GetControllersInitialDelay(0.2f)
{
	PrimaryComponentTick.bCanEverTick = true;
	//Load blueprint class into VRDebugWidget variable
	static ConstructorHelpers::FClassFinder<AActor> VRDebug(TEXT("Blueprint'/VRDebugPlugin/Blueprints/BP_VRDebug.BP_VRDebug_C'"));
	if (VRDebug.Succeeded()) VRDebugWidget = VRDebug.Class;

	//Load the blueprint class into LaserBeam variable
	static ConstructorHelpers::FClassFinder<AActor> LaserBeamClass(TEXT("Blueprint'/VRDebugPlugin/Blueprints/BP_LaserBeam.BP_LaserBeam_C'"));
	if (LaserBeamClass.Succeeded()) LaserBeam = LaserBeamClass.Class;

	//Load the blueprint class into LaserBeam variable
	static ConstructorHelpers::FClassFinder<AActor> BlueprintLogClass(TEXT("Blueprint'/VRDebugPlugin/Blueprints/BP_BlueprintLog.BP_BlueprintLog_C'"));
	if (BlueprintLogClass.Succeeded()) BlueprintLog = BlueprintLogClass.Class;

	static ConstructorHelpers::FClassFinder<UDebugWidgetView> ConsoleWidgetClass(TEXT("WidgetBlueprint'/VRDebugPlugin/UMG/ConsoleCommands/W_Console.W_Console_C'"));
	if (ConsoleWidgetClass.Succeeded()) VRDebugWidgetViews.Add("Console", ConsoleWidgetClass.Class);

	static ConstructorHelpers::FClassFinder<UDebugWidgetView> ActorFunctionClass(TEXT("WidgetBlueprint'/VRDebugPlugin/UMG/Reflection/W_RunFunction.W_RunFunction_C'"));
	if (ActorFunctionClass.Succeeded()) VRDebugWidgetViews.Add("Actor Functions", ActorFunctionClass.Class);

	static ConstructorHelpers::FClassFinder<UDebugWidgetView> ActorInspectorClass(TEXT("WidgetBlueprint'/VRDebugPlugin/UMG/Reflection/W_ActorInspector.W_ActorInspector_C'"));
	if (ActorInspectorClass.Succeeded()) VRDebugWidgetViews.Add("Actor Inspector", ActorInspectorClass.Class);

	static ConstructorHelpers::FClassFinder<UDebugWidgetView> LoadMapClass(TEXT("WidgetBlueprint'/VRDebugPlugin/UMG/LoadLevels/W_LoadLevels.W_LoadLevels_C'"));
	if (LoadMapClass.Succeeded()) VRDebugWidgetViews.Add("Maps", LoadMapClass.Class);

	static ConstructorHelpers::FClassFinder<UDebugWidgetView> MovementsClass(TEXT("WidgetBlueprint'/VRDebugPlugin/UMG/Movements/W_Movements.W_Movements_C'"));
	if (MovementsClass.Succeeded()) VRDebugWidgetViews.Add("Movements", MovementsClass.Class);

	static ConstructorHelpers::FClassFinder<UDebugWidgetView> OutputLogClass(TEXT("WidgetBlueprint'/VRDebugPlugin/UMG/Logs/W_OutputLog.W_OutputLog_C'"));
	if (OutputLogClass.Succeeded()) VRDebugWidgetViews.Add("Output Log", OutputLogClass.Class);

#if WITH_EDITOR
	GetClassProperties();
#endif
}

void UVRDebugComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_BUILD_SHIPPING
	if(!bIncludeInShipping)	DestroyComponent();
#endif

	if (GetOwner())
	{
		Owner = GetOwner();
		if (UActorComponent* Camera = Owner->GetComponentByClass(UCameraComponent::StaticClass()))
		{
			OwnerCamera = Cast<UCameraComponent>(Camera);
		}
	}

	FTimerHandle ControllerHandle;
	FTimerHandle InputHandle;
	FTimerHandle AutoSpawnHandle;
	FTimerHandle BlueprintLogHandle;
	GetWorld()->GetTimerManager().SetTimer(ControllerHandle, this, &UVRDebugComponent::GetMotionController, GetControllersInitialDelay, false);
	if(bShouldBindInputs) GetWorld()->GetTimerManager().SetTimer(InputHandle, this, &UVRDebugComponent::BindInputs, GetControllersInitialDelay+0.1f, false);
    if(bAutoSpawn) GetWorld()->GetTimerManager().SetTimer(AutoSpawnHandle, this, &UVRDebugComponent::SpawnVRDebugMenu, GetControllersInitialDelay + 0.2f, false);	
	if (bAutoSpawnBlueprintLog) GetWorld()->GetTimerManager().SetTimer(BlueprintLogHandle, this, &UVRDebugComponent::SpawnBlueprintLog, GetControllersInitialDelay + 0.2f, false);
}

void UVRDebugComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UVRDebugComponent::GetMenuLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const
{
	if (OwnerCamera)
	{
		const FVector ForwardVector = UKismetMathLibrary::GetForwardVector(OwnerCamera->GetComponentRotation());
		const FVector ForwardVectorNoHeight = FVector(ForwardVector.X, ForwardVector.Y, 0);
		const FVector RotatedForwardVector = UKismetMathLibrary::RotateAngleAxis(ForwardVectorNoHeight, SpawnAtAngleFromCamera, FVector::UpVector);
		OutLocation = OwnerCamera->GetComponentLocation() + (RotatedForwardVector * SpawnDistanceFromCamera);
		OutRotation = UKismetMathLibrary::FindLookAtRotation(OutLocation, OwnerCamera->GetComponentLocation());
	}
	else if (Owner)
	{
		UE_LOG(VRDebugLog, Warning, TEXT("Could not get camera component for VRDebugComponent inside its parent"));
		OutLocation = Owner->GetActorLocation() + (Owner->GetActorForwardVector() * SpawnDistanceFromCamera);
		OutRotation = UKismetMathLibrary::FindLookAtRotation(OutLocation, Owner->GetActorLocation());
	}
	else UE_LOG(VRDebugLog, Error, TEXT("Could not get owner actor for VRDebugComponent"));
}

#pragma region SpawnFunctions
void UVRDebugComponent::SpawnVRDebugMenu_Implementation()
{
	FVector NewMenuLocation;
	FRotator NewMenuRotation;
	GetMenuLocationAndRotation(NewMenuLocation, NewMenuRotation);
	
	if (VRDebugWidget)
	{
		UE_LOG(VRDebugLog, Log, TEXT("Spawn VR Debug Widget"));
		const FTransform ActorInitialTrans = FTransform(NewMenuRotation, NewMenuLocation, FVector::OneVector);
		VRDebugActor = GetOwner()->GetWorld()->SpawnActorDeferred<AMovableActor>(VRDebugWidget, ActorInitialTrans);
		VRDebugActor->VRDebugComponent = this;
		VRDebugActor->FinishSpawning(ActorInitialTrans);
		if (!bAutoSpawnBlueprintLog) SpawnLaserBeams();
		VRDebugActor->ActorShown();
	}
	else
	{
		UE_LOG(VRDebugLog, Error, TEXT("The VR Debug Widget actor hasn't been set. Make sure the property is set inside the VRDebugComponent details panel."));
	}
}

void UVRDebugComponent::SpawnLaserBeams()
{
	if (LaserBeam->IsValidLowLevel())
	{
		if (MCLeft)
		{
			LaserBeamLeft = Owner->GetWorld()->SpawnActorDeferred<ALaserBeam>(LaserBeam, MCLeft->GetComponentTransform());
			LaserBeamLeft->ClickKey = LeftHandWidgetClickKey;
			LaserBeamLeft->Hand = EControllerHand::Left;
			LaserBeamLeft->SetOwner(MCLeft->GetOwner());
			LaserBeamLeft->bFreeFlyingEnabled = bFreeFlyingEnabled;
			LaserBeamLeft->VRDebugComponent = this;
			LaserBeamLeft->FinishSpawning(MCLeft->GetComponentTransform());
			LaserBeamLeft->AttachToComponent(MCLeft, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
		}
		else
		{
			UE_LOG(VRDebugLog, Warning, TEXT("Left motion controller is not valid inside of SpawnLaserBeams."));
		}

		if (MCRight)
		{
			LaserBeamRight = Owner->GetWorld()->SpawnActorDeferred<ALaserBeam>(LaserBeam, MCRight->GetComponentTransform());
			LaserBeamRight->ClickKey = RightHandWidgetClickKey;
			LaserBeamRight->Hand = EControllerHand::Right;
			LaserBeamRight->bFreeFlyingEnabled = bFreeFlyingEnabled;
			LaserBeamRight->SetOwner(MCRight->GetOwner());
			LaserBeamRight->VRDebugComponent = this;
			LaserBeamRight->FinishSpawning(MCRight->GetComponentTransform());
			LaserBeamRight->AttachToComponent(MCRight, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
		}
		else
		{
			UE_LOG(VRDebugLog, Warning, TEXT("Right motion controller is not valid inside of SpawnLaserBeams."));
		}
	}
}

void UVRDebugComponent::SpawnBlueprintLog()
{
	FVector BlueprintLogLocation;
	FRotator BlueprintLogRotation;

	if (OwnerCamera)
	{
		FVector ForwardVector = UKismetMathLibrary::GetForwardVector(OwnerCamera->GetComponentRotation());
		FVector ForwardVectorNoHeight = FVector(ForwardVector.X, ForwardVector.Y, 0);
		FVector RotatedForwardVector = UKismetMathLibrary::RotateAngleAxis(ForwardVectorNoHeight, BlueprintLogSpawnAngle, FVector::UpVector);
		BlueprintLogLocation = OwnerCamera->GetComponentLocation() + (RotatedForwardVector * BlueprintLogSpawnDistanceFromCamera);
		BlueprintLogRotation = UKismetMathLibrary::FindLookAtRotation(BlueprintLogLocation, OwnerCamera->GetComponentLocation());
	}
	else if (Owner)
	{
		FVector ForwardVector = Owner->GetActorForwardVector();
		FVector ForwardVectorNoHeight = FVector(ForwardVector.X, ForwardVector.Y, 0);
		FVector RotatedForwardVector = UKismetMathLibrary::RotateAngleAxis(ForwardVectorNoHeight, BlueprintLogSpawnAngle, FVector::UpVector);
		BlueprintLogLocation = Owner->GetActorLocation() + (RotatedForwardVector * BlueprintLogSpawnDistanceFromCamera);
		BlueprintLogRotation = UKismetMathLibrary::FindLookAtRotation(BlueprintLogLocation, Owner->GetActorLocation());
	}

	FTransform ActorInitialTrans = FTransform(BlueprintLogRotation, BlueprintLogLocation, FVector::OneVector);
	AMovableActor* BlueprintLogActor = GetOwner()->GetWorld()->SpawnActorDeferred<AMovableActor>(BlueprintLog, ActorInitialTrans);
	BlueprintLogActor->VRDebugComponent = this;
	BlueprintLogActor->FinishSpawning(ActorInitialTrans);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &UVRDebugComponent::SpawnLaserBeams, GetControllersInitialDelay+0.1f, false);
}

void UVRDebugComponent::SpawnMenu()
{
	if (!VRDebugActor)
	{
		SpawnVRDebugMenu();
		return;
	}

	if (VRDebugActor->IsHidden())
	{
		VRDebugActor->SetActorHiddenInGame(false);
		VRDebugActor->ActorShown();
	}
}

void UVRDebugComponent::DestroyMenu()
{
	if(VRDebugActor)
	{
		if (!VRDebugActor->IsHidden())
		{
			EControllerHand Hand = EControllerHand::Right;
			if(OpenMenuKey.GetDisplayName().ToString().ToLower().Contains("left")) Hand = EControllerHand::Left;
			VRDebugActor->StartHideActor(Hand);
			VRDebugActor->FinishHideActor();
		}
	}
}
#pragma endregion

#pragma region Inputs
void UVRDebugComponent::BindInputs()
{
	if (Owner)
	{
		APlayerController* PlayerController = Owner->GetWorld()->GetFirstPlayerController();
		if(PlayerController)
		{
			Owner->EnableInput(PlayerController);
			if (Owner->InputComponent->IsValidLowLevel())
			{
				Owner->InputComponent->BindKey(OpenMenuKey, EInputEvent::IE_Pressed, this, &UVRDebugComponent::OnSpawnMenuKeyPressed).bConsumeInput = false;
				Owner->InputComponent->BindKey(OpenMenuKey, EInputEvent::IE_Released, this, &UVRDebugComponent::OnSpawnMenuKeyReleased). bConsumeInput = false;
			}
		}
	}
	else
	{
		UE_LOG(VRDebugLog, Warning, TEXT("Could not get parent of VRDebugComponent"));
	}
}

void UVRDebugComponent::OnSpawnMenuKeyPressed()
{
	if (!VRDebugActor->IsValidLowLevel()) return;

	if (!VRDebugActor->IsHidden())
	{
		EControllerHand Hand = EControllerHand::Right;
		if(OpenMenuKey.GetDisplayName().ToString().ToLower().Contains("left")) Hand = EControllerHand::Left;
		VRDebugActor->StartHideActor(Hand);
	}
}

void UVRDebugComponent::OnSpawnMenuKeyReleased()
{
	if (!VRDebugActor->IsValidLowLevel())
	{
		SpawnVRDebugMenu();
		return;
	}

	if (VRDebugActor->IsHidden())
	{
		VRDebugActor->SetActorHiddenInGame(false);
		VRDebugActor->ActorShown();
	}
	else
	{
		VRDebugActor->FinishHideActor();
	}
}
#pragma endregion

#pragma region MotionControllerFunctions
void UVRDebugComponent::GetMotionController()
{
	if (bLookForMotionControllers) GetMotionControllerFromWorld();
	else GetMotionControllerFromName();
	if (VRDebugActor && !VRDebugActor->IsHidden()) SpawnLaserBeams();
}

void UVRDebugComponent::GetMotionControllerFromWorld()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetOwner()->GetWorld(), AActor::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		TArray<UActorComponent*> Components;
		Actor->GetComponents(UMotionControllerComponent::StaticClass(), Components, false);
		for (UActorComponent* Comp : Components)
		{
			if (UMotionControllerComponent * MC = Cast<UMotionControllerComponent>(Comp))
			{
				if (MC->MotionSource.ToString().Contains("Left"))
					MCLeft = MC;
				else if (MC->MotionSource.ToString().Contains("Right"))
					MCRight = MC;
			}
		}
	}
}

void UVRDebugComponent::GetMotionControllerFromName()
{
	if (!GetOwner()) return;

	UClass* MyClass = GetOwner()->GetClass();
	if (!MyClass) return;

	for (UEProperty* Property = MyClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		UEObjectProperty* ObjProperty = UECast<UEObjectProperty>(Property);
		if (ObjProperty)
		{
			UObject* Object = ObjProperty->GetObjectPropertyValue(ObjProperty->ContainerPtrToValuePtr<UObject>(GetOwner()));
			if (Object)
			{
				UMotionControllerComponent* MotionController = Cast<UMotionControllerComponent>(Object);
				if (MotionController && MotionController->MotionSource.ToString().Contains("Left"))
				{
					MCLeft = MotionController;
				}
			}

			if (ObjProperty->GetFName() == MotionControllerLeft && ObjProperty->PropertyClass->IsChildOf(AActor::StaticClass()) && Object)
			{
				AActor* Actor = Cast<AActor>(Object);
				MCLeft = GetMotionControllerFromActor(Actor, TEXT("Left"));
			}
		}
	}
	
	for (UEProperty* Property = MyClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		UEObjectProperty* ObjProperty = UECast<UEObjectProperty>(Property);
		if (ObjProperty)
		{
			UObject* Object = ObjProperty->GetObjectPropertyValue(ObjProperty->ContainerPtrToValuePtr<UObject>(GetOwner()));
			if (Object)
			{
				UMotionControllerComponent* MotionController = Cast<UMotionControllerComponent>(Object);
				if (MotionController && MotionController->MotionSource.ToString().Contains("Right"))
				{
					MCRight = MotionController;
				}
			}

			if (ObjProperty->GetFName() == MotionControllerRight && ObjProperty->PropertyClass->IsChildOf(AActor::StaticClass()) && Object)
			{
				AActor* Actor = Cast<AActor>(Object);
				MCRight = GetMotionControllerFromActor(Actor, TEXT("Right"));
			}
		}
	}
}

UMotionControllerComponent* UVRDebugComponent::GetMotionControllerFromActor(AActor* Actor, FName MotionSource)
{
	TArray<UActorComponent*> Components;
	Actor->GetComponents(UMotionControllerComponent::StaticClass(), Components, false);
	for (UActorComponent* Comp : Components)
	{
		if (UMotionControllerComponent * MC = Cast<UMotionControllerComponent>(Comp))
		{
			if (MC->MotionSource.ToString().Contains(MotionSource.ToString())) return MC;
		}
	}

	return nullptr;
}
#pragma endregion

#pragma region CommandsFunctions
UCommandBase* UVRDebugComponent::CreateNewCommand(TSubclassOf<UCommandBase> CommandClass)
{
	UCommandBase* Command = NewObject<UCommandBase>(this, CommandClass);
	if (Command->IsValidLowLevel())
	{
		return Command;
	}
	else
		return nullptr;
}

void UVRDebugComponent::PerformAction(UCommandBase* Command)
{
	if (Command->IsValidLowLevel())
	{
		CommandsRun.Add(Command);
		Command->PerformAction();
	}
}

void UVRDebugComponent::UndoLastCommand()
{
	if (CommandsRun.Num() > 0)
	{
		int32 LastIndex = CommandsRun.Num()-1;
		UCommandBase* Command = CommandsRun[LastIndex];
		CommandsRun.RemoveAt(LastIndex);
		if (Command->IsValidLowLevel()) Command->Undo();
	}
}
#pragma endregion

#if WITH_EDITOR 
void UVRDebugComponent::PostInitProperties()
{	
	Super::PostInitProperties();
	GetBlueprintVariables();
}

void UVRDebugComponent::GetClassProperties()
{
	UClass* Class = GetClass();
	if (Class)
	{
		for (TFieldIterator<UEProperty> PropertyIterator(Class); PropertyIterator; ++PropertyIterator)
		{
			UEProperty* Property = *PropertyIterator;
			PropertiesMap.Add(Property->GetFName().ToString(), false);
		}
	}
}

void UVRDebugComponent::GetBlueprintVariables()
{
	MotionControllerCandidates.Empty();

	if (!GetOwner())
	{
		return;
	}

	UClass* MyClass = GetOwner()->GetClass();
	if (!MyClass)
	{
		UE_LOG(VRDebugLog, Error, TEXT("Could not get class of owner"));
		return;
	}

	for (UEProperty* Property = MyClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		UEObjectProperty* ObjProperty = UECast<UEObjectProperty>(Property);
		if (ObjProperty)
		{
			UObject* Object = ObjProperty->GetObjectPropertyValue(ObjProperty->ContainerPtrToValuePtr<UObject>(GetOwner()));
			if (Object)
			{
				UMotionControllerComponent* MotionController = Cast<UMotionControllerComponent>(Object);
				if (MotionController)
				{
					MotionControllerCandidates.Add(ObjProperty->GetFName());
				}
			}

			if (ObjProperty->PropertyClass->IsChildOf(AActor::StaticClass()))
			{
				if (!ObjProperty->IsNative())
				{
					MotionControllerCandidates.Add(ObjProperty->GetFName());
				}
			}
		}
	}
}

void UVRDebugComponent::SaveProperties()
{	
	//Path to config file
	FString Path = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / TEXT("VRDebugComponent.ini");
	SaveConfig(CPF_Config, *Path);
	UE_LOG(VRDebugLog, Log, TEXT("Saved VR Debug Component Properties"));
}

void UVRDebugComponent::LoadProperties()
{
	//Path to config file
	FString Path = IPluginManager::Get().FindPlugin("VRDebugPlugin")->GetBaseDir() / TEXT("Config") / TEXT("VRDebugComponent.ini");
	//Load config file for the component blueprint instance from which the load properties button is clicked
	PreEditChange(NULL);
	LoadConfig(UVRDebugComponent::StaticClass(), *Path);
	PostEditChange();

	//Update all the actors in the level that may have this component on them
	TArray<UVRDebugComponent*> VRDebugComponents = GetAllVRDebugComponent();
	for (UVRDebugComponent* Component : VRDebugComponents)
	{
		PreEditChange(NULL);
		Component->LoadConfig(UVRDebugComponent::StaticClass(), *Path);
		PostEditChange();
	}

	UE_LOG(VRDebugLog, Log, TEXT("Loaded VR Debug Component Properties"));
}

bool UVRDebugComponent::CheckIfInEditorAndPIE()
{
	if (!IsInGameThread())
	{
		UE_LOG(VRDebugLog, Error, TEXT("You are not on the main thread."));
		return false;
	}
	if (!GIsEditor)
	{
		UE_LOG(VRDebugLog, Error, TEXT("You are not in the Editor."));
		return false;
	}
	if (GEditor->PlayWorld || GIsPlayInEditorWorld)
	{
		UE_LOG(VRDebugLog, Error, TEXT("The Editor is currently in a play mode."));
		return false;
	}
	return true;
}

TArray<AActor*> UVRDebugComponent::GetAllLevelActors()
{
	TArray<AActor*> Result;
	UWorld* EditorWorld = GEditor ? GEditor->GetEditorWorldContext(false).World() : nullptr;
	if (!EditorWorld) return Result;

	if (CheckIfInEditorAndPIE())
	{
		//Default iterator only iterates over active levels.
		const EActorIteratorFlags Flags = EActorIteratorFlags::SkipPendingKill;
		for (TActorIterator<AActor> It(EditorWorld, AActor::StaticClass(), Flags); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor->IsEditable() &&
				Actor->IsListedInSceneOutliner() &&					// Only add actors that are allowed to be selected and drawn in editor
				!Actor->IsTemplate() &&								// Should never happen, but we never want CDOs
				!Actor->HasAnyFlags(RF_Transient) &&				// Don't add transient actors in non-play worlds
				!Actor->IsA(AWorldSettings::StaticClass()))			// Don't add the WorldSettings actor, even though it is technically editable
			{
				Result.Add(*It);
			}
		}
	}

	return Result;
}

TArray<UVRDebugComponent*> UVRDebugComponent::GetAllVRDebugComponent()
{
	TArray<UVRDebugComponent*> VRDebugComponents;
	TArray<AActor*> LevelActors = GetAllLevelActors();
	for (AActor* Actor : LevelActors)
	{
		TArray<UActorComponent*> Components;
		Actor->GetComponents(this->StaticClass(), Components, false);
		for (UActorComponent* ActorComponent : Components)
		{
			VRDebugComponents.Add(Cast<UVRDebugComponent>(ActorComponent));
		}
	}
	return VRDebugComponents;
}
#endif