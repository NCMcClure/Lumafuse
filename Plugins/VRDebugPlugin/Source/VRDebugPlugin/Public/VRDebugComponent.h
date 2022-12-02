// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovableActor.h"
#include "CommandBase.h"
#include "LaserBeam.h"
#include "UMG/DebugWidgetView.h"
#include "Runtime/InputCore/Classes/InputCoreTypes.h"
#include "VRDebugWidgetFL.h"
#include "VRDebugComponent.generated.h"

class UMotionControllerComponent;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetClicked, bool, bButtonPressed);

USTRUCT(BlueprintType)
struct FVRWidgetColourCustomisation
{
	GENERATED_BODY()

	FVRWidgetColourCustomisation()
	:	  WidgetTextColour(FLinearColor::White)
		, WidgetOutputLogWarningColour(FLinearColor(0.871367, 0.658375, 0.000304))
		, WidgetOutputLogErrorColour(FLinearColor(0.715693, 0.043735, 0.051269))
		, WidgetBackgroundColour(0.048172, 0.048172, 0.048172)
		, WidgetButtonsColour(0.084376, 0.391573, 0.084376)
		, WidgetButtonsHoverColour(0.107023, 0.491021, 0.107023)
		, WidgetTabsColour(0.048172, 0.048172, 0.048172)
		, WidgetLines(0.116971, 0.116971, 0.116971)
		, WidgetTabSelected(FLinearColor::White)
		, PrintMessagesColour(FLinearColor::White)
		, PrintMessagesActorColour(FLinearColor::Red)
	{}

	/** This the colour applied to all the text in the widget*/
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetTextColour;

	/** This is the colour applied to all the text warnings in the output log */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetOutputLogWarningColour;

	/** This is the colour applied to all the text errors in the output log */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetOutputLogErrorColour;

	/** This is the widget background colour */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetBackgroundColour;

	/** This is the widget colour for buttons as well as line separators */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetButtonsColour;

	/** This is the widget colour for buttons as well as line separators */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetButtonsHoverColour;

	/** This is the widget colour of the tabs as well as titles */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetTabsColour;

	/** This is the widget colour of the tabs as well as titles */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetLines;

	/** This is the widget colour of the tabs as well as titles */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor WidgetTabSelected;

	/** This is the colour used for displaying the print messages text */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor PrintMessagesColour;

	/** This is the colour used for displaying the actor who printed the message text */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FLinearColor PrintMessagesActorColour;
};

UCLASS(ClassGroup=(Debug), meta=(BlueprintSpawnableComponent, DisplayName= "VR Debug Component"), Config=Custom)
class VRDEBUGPLUGIN_API UVRDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVRDebugComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

#pragma region VRDebugProperties
	/**
	 * Should the VR Debug Component be used in a shipping build? If false the component will auto destroy
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Properties")
	bool bIncludeInShipping;
	
	/**
	 * Should the component bind inputs on its own? If so it will get the first player controller that it can find in the world. If you are using a custom player controller this should be false and you'll have to call the function BindInputs passing in the player controller.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Properties")
	bool bShouldBindInputs;
	
	/**
	 * Should free flying be enabled? If enabled when the VR Debug Widget is on you can point in any direction press the trigger and free fly.
	 * Free-flying still requires the W_Movements view to be added to the list of VR Debug Widget Views
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Properties")
	bool bFreeFlyingEnabled = true;
#pragma endregion
	
#pragma region VRDebugWidget
    /**
     * Should the widget auto spawn?
     */
    UPROPERTY(Config, EditAnywhere, Category = "VR Debug Widget", meta = (DisplayName="Should Auto Spawn?"))
	bool bAutoSpawn;
    
	/**
	 * The key that brings up the VR Debug Actor
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Widget")
	FKey OpenMenuKey;

	/**
	 * This is the collision channel used to interact with the VR Debug Widget. You can create a custom channel in the project settings if you have troubles interacting with the widget in your leve. 
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget")
	TEnumAsByte<ECollisionChannel> WidgetInteractionChannel;
	
	/**
	 * This is the overall size of the actor in which the widget component for the debug menu is placed. 
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget")
	float VRWidgetScale;

	/**
	 * The forward distance from the camera at which the VR Debug Widget actor will be spawned.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget")
	float SpawnDistanceFromCamera;

	/**
	 * At what angle from the camera forward vector should the widget spawn? 
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget")
	float SpawnAtAngleFromCamera;
	
	/**
	 * The key that will be used to interact with the widget. The key will be used as a left mouse click.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Widget")
	FKey LeftHandWidgetClickKey;

	/**
	 * The key that will be used to interact with the widget. The key will be used as a left mouse click.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Widget")
	FKey RightHandWidgetClickKey;
#pragma endregion

#pragma region VRDebugWidgetClasses
	/**
	 * This should not be changed unless you customise the default VR Debug Widget. The property is only exposed for special customisation cases.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Widget Views")
	TSubclassOf<AMovableActor> VRDebugWidget;

	/**
	 * The list of widget views the VR debug widget will spawn.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "VR Debug Widget Views")
	TMap<FString, TSubclassOf<UDebugWidgetView>> VRDebugWidgetViews;

	/**
	 * This is the class of the laser beam object spawned. The laser beam is used to interact with the VR Debug Widget.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Widget Views")
	TSubclassOf<ALaserBeam> LaserBeam;

	/**
	 * This should not be changed unless you customise the default BlueprintLog. The property is only exposed for special customisation cases.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "VR Debug Widget Views")
	TSubclassOf<AMovableActor> BlueprintLog;
#pragma endregion

#pragma region Screenshot
	/**
	 * If left empty this output folder will be the project folder if running in the editor, the WindowsNoEditor folder on a Windows build and /sdcard/UE4Game/GameName/ on android.
	 * If you have enabled Use Relative Path you can enter the path relative to the project folder, WindowNoEditor when built on Windows and /sdcard/UE4Game/GameName/ on android.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "In-game Screenshot", meta = (UIMin = 1.0, UIMax = 10.0))
	FString ScreenshotOutputFolder;

	/**
	 * Should the screenshot be saved to a relative folder path? 
	 */ 
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "In-game Screenshot")
	bool bUseRelativePath;

	/**
	 * Should we try to create the folder if it does not exist?
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "In-game Screenshot")
	bool bTryCreateFolder;
	
	/**
	 * The countdown used by the screenshot actor before screenshot what you are currently seeing
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "In-game Screenshot", meta = (UIMin = 1.0, UIMax = 10.0))
	float CountDown;

	/**
	 * The camera field of view used by the screenshot actor scene capture component camera
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "In-game Screenshot", meta = (UIMin = 60, UIMax = 180))
	int32 CameraFieldOfView;

	/**
	 * The screenshot resolution on the X
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "In-game Screenshot", meta = (UIMin = 1.0, UIMax = 4096))
	int32 ScreenshotResolutionX;

	/**
	 * The screenshot resolution on the Y
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "In-game Screenshot", meta = (UIMin = 1.0, UIMax = 4096))
	int32 ScreenshotResolutionY;
#pragma endregion

#pragma region VRDebugWidgetCustomisation
	/**
	 * The console command widget column width for each category.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	float ConsoleCategoryColumnWidth;

	/**
	 * This value is the overall font size that will be used in the widget where possible. NOTE: Some font sizes are limited to a max number to avoid breaking the UI.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation", meta = (UIMin = "20", UIMax = "50", ClampMin = "20", ClampMax = "50"))
	int32 WidgetFontSize;

	/**
	 * The widget font type used in the VR debug widget.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	EFontTypeFace WidgetFontType;
	
	/**
	 * The VR debug widget colours.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "VR Debug Widget Customisation")
	FVRWidgetColourCustomisation VRWidgetColourCustomisation;
#pragma endregion

#pragma region BlueprintLog/OutputLog
	/**
	 * The maximum number of messages that the output log should hold at any time.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log", meta = (UIMin = "100", UIMax = "100000", ClampMin = "100", ClampMax = "100000"))
	int32 OutputLogMaxMessages;

	/**
	 * The maximum number of messages the blueprint log should hold at any time. First entry of the list gets deleted when the number is exceeded.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log", meta = (UIMin = "1"))
	int32 BlueprintLogMaxMessages;

	/**
	 * The forward distance from the camera at which the Blueprint Log actor will be spawned.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log")
	float BlueprintLogSpawnDistanceFromCamera;

	/**
	 * The angle in degrees at which the Blueprint Log will be spawned. E.g. 45 degrees will spawn the Blueprint Log at 45 degrees to your right.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log")
	float BlueprintLogSpawnAngle;

	/**
	 * This is the minimum width for the blueprint log.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log", meta = (UIMin="600"))
	float BlueprintLogMinWidth;

	/**
	 * This is the maximum width for the blueprint log.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log", meta = (UIMin="600"))
	float BlueprintLogMaxWidth;
	
	/**
	 * Should the blueprint print statements widget be spawned as soon as we start playing? If this is true there would be no way to switch off the widget and will always be available by your side
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log")
	bool bAutoSpawnBlueprintLog;

	/**
	 * Should the Blueprint Log Widget draw on top of everything?
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log")
	bool bBlueprintLogDrawsOnTop;

	/**
	 * Should the print messages show their owner. This allows you to understand who printed the message.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Blueprint Log/Output Log")
	bool bPrintMessagesShowOwner;
#pragma endregion

#pragma region MotionControllers
	/**
	 * Should we actually traverse the whole world and look for the first two instances of motion controller components?
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Motion Controllers")
	bool bLookForMotionControllers;

	/**
	 * The component will delay trying to get the controllers by this amount. Useful if the controllers variables are set on begin play.
	 */
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Motion Controllers")
	float GetControllersInitialDelay;

	/**
	 * The name of the variable that will be inspected at runtime to retrieve the left motion controller
	 */
	UPROPERTY(EditAnywhere, Category = "Motion Controllers", meta = (EditCondition = "!bLookForMotionControllers"))
	FName MotionControllerLeft;

	/**
	 * The name of the variable that will be inspected at runtime to retrieve the Right motion controller
	 */
	UPROPERTY(EditAnywhere, Category = "Motion Controllers", meta = (EditCondition="!bLookForMotionControllers"))
	FName MotionControllerRight;
#pragma endregion

#pragma region BlueprintAccessVars
	/**
	 * Runtime reference to the VR camera
	 */
	UPROPERTY(BlueprintReadOnly, Category = "VR Debug Component")
	UCameraComponent* OwnerCamera;
	
	/**
	 * Left hand laser beam
	 */
	UPROPERTY(BlueprintReadOnly, Category = "VR Debug Component")
	ALaserBeam* LaserBeamLeft;
	
	/**
	 * Right hand laser beam
	 */
	UPROPERTY(BlueprintReadOnly, Category = "VR Debug Component")
	ALaserBeam* LaserBeamRight;
	
	/**
	 * Runtime reference to the left motion controller
	 */
	UPROPERTY(BlueprintReadOnly, Category = "VR Debug Component")
	UMotionControllerComponent* MCLeft;
	
	/**
	 * Runtime reference to the right motion controller
	 */
	UPROPERTY(BlueprintReadOnly, Category = "VR Debug Component")
	UMotionControllerComponent* MCRight;
	
	/**
	 * The runtime free-flying speed variable set by the W_Movements widget
	 */
	UPROPERTY(BlueprintReadWrite, Category = "VR Debug Component")
	float FreeFlyingSpeed;
#pragma endregion

#pragma region Public_Internals
	/**
	 * Map containing all the properties of this class and their ticked state
	 */
	UPROPERTY()
	TMap<FString, bool> PropertiesMap;

	/**
	 * Array of blueprint variable names
	 */
	static TArray<FName> MotionControllerCandidates;

	/**
	 * Array of commands that has been run and can be undone
	 */
	UPROPERTY()
	TArray<UCommandBase*> CommandsRun;

	/**
	 * Dispatcher fired by button presses on the widgets, this allow us to artificially release the trigger and let the button clicks be very precise
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "VR Debug Component")
	FOnWidgetClicked OnWidgetClicked;
#pragma endregion

public:
	/**
	 * Spawn the VR Debug widget menu
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "VR Debug")
	void SpawnVRDebugMenu();

	/**
	 * Spawn the menu.
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Debug")
	void SpawnMenu();
	
	/**
	 * Destroy the menu.
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Debug")
	void DestroyMenu();
	
	/**
	 * Create a new command.
	 * @param CommandClass The class that represents a command.
	 * @return The newly created command.
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Debug|Undo")
	UCommandBase* CreateNewCommand(TSubclassOf<UCommandBase> CommandClass);

	/**
	 * Perform an action
	 * @param Command The command to perform.
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Debug|Undo")
	void PerformAction(UCommandBase* Command);

	/**
	 * Undoes the last run command.
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Debug|Undo")
	void UndoLastCommand();

	/**
	 * Bind all the keys to KeyPressed
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Debug|Inputs")
	void BindInputs();

#if WITH_EDITOR
private:
	virtual void PostInitProperties() override;
	
	/** Gets an array of all the possible motion controller related blueprint variables */
	void GetBlueprintVariables();
	
	/** Get all the class properties - used by the save and load system */
	void GetClassProperties();
	
	/** Get all the actors in the current level within the editor */
	TArray<AActor*> GetAllLevelActors();
	
	/** Get all VRDebugComponent in the level */
	TArray<UVRDebugComponent*> GetAllVRDebugComponent();
	
	/** Are we working in the editor? */
	bool CheckIfInEditorAndPIE();
	
public:
	/** Save the properties ticked */
	void SaveProperties();
	
	/** Load all available properties */
	void LoadProperties();
#endif

private:
	/**Checks if the right keys were pressed and calls spawn vr debug menu function*/
	void OnSpawnMenuKeyPressed();
	
	/**Finish to hide the vr widget*/
	void OnSpawnMenuKeyReleased();
	
	/** Traverses all the actors and their components in the world to get a left and right motion controller */
	void GetMotionControllerFromWorld();
	
	/** Uses the stored motion controller name to get the right blueprint property */
	void GetMotionControllerFromName();
	
	/** Get the motion controller component from the given actor (if any) with the specified motion source */
	UMotionControllerComponent* GetMotionControllerFromActor(AActor* Actor, FName MotionSource);
	
	/** Spawn the laser beams for the left and right motion controller */
	void SpawnLaserBeams();
	
	/** Get the menu location and rotation when spawning it or showing it */
	void GetMenuLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const;
	
	/** Function to fire GetMotionControllerFromWorld with delay */
	void GetMotionController();
	
	/** If bAutoSpawnBlueprintLog is true then this function will spawn the blueprint log without the user having to press anything */
	void SpawnBlueprintLog();
	
private:
	/** Reference to the VR debug widget actor spawned */
	UPROPERTY()
	AMovableActor* VRDebugActor;
	
	/** Runtime reference to the owner of this component */
	UPROPERTY()
	AActor* Owner;
};

