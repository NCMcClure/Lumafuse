// Copyright 2017-2019 David Romanski (Socke). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Misc/Base64.h"
#include "Misc/SecureHash.h"
#include "Misc/AES.h"
#include "Modules/ModuleManager.h"
#include "SocketClient.generated.h"

USTRUCT(BlueprintType)
struct FFileFunctionsSocketClientOpenFile
{
	GENERATED_USTRUCT_BODY()

		FArchive* writer = nullptr;

};

UENUM(BlueprintType)
enum class EFileFunctionsSocketClientDirectoryType : uint8
{
	E_gd	UMETA(DisplayName = "Game directory"),
	E_ad 	UMETA(DisplayName = "Absolute directory")
};


UENUM(BlueprintType)
enum class EFileFunctionsSocketClientEncodingOptions : uint8
{
	E_AutoDetect	UMETA(DisplayName = "AutoDetect"),
	E_ForceAnsi 	UMETA(DisplayName = "ForceAnsi"),
	E_ForceUnicode	UMETA(DisplayName = "ForceUnicode"),
	E_ForceUTF8 	UMETA(DisplayName = "ForceUTF8"),
	E_ForceUTF8WithoutBOM	UMETA(DisplayName = "ForceUTF8WithoutBOM")
};

UENUM(BlueprintType)
enum class EReceiveFilterClient : uint8
{
	E_SAB 	UMETA(DisplayName = "Message And Bytes"),
	E_S		UMETA(DisplayName = "Message"),
	E_B		UMETA(DisplayName = "Bytes")

};


UENUM(BlueprintType)
enum class ESocketPlatformClient : uint8
{
	E_SSC_SYSTEM		UMETA(DisplayName = "System"),
	E_SSC_DEFAULT 		UMETA(DisplayName = "Auto"),
	E_SSC_WINDOWS		UMETA(DisplayName = "WINDOWS"),
	E_SSC_MAC			UMETA(DisplayName = "MAC"),
	E_SSC_IOS			UMETA(DisplayName = "IOS"),
	E_SSC_UNIX			UMETA(DisplayName = "UNIX"),
	E_SSC_ANDROID		UMETA(DisplayName = "ANDROID"),
	E_SSC_PS4			UMETA(DisplayName = "PS4"),
	E_SSC_XBOXONE		UMETA(DisplayName = "XBOXONE"),
	E_SSC_HTML5			UMETA(DisplayName = "HTML5"),
	E_SSC_SWITCH		UMETA(DisplayName = "SWITCH")

};

UENUM(BlueprintType)
enum class ESocketClientIPType : uint8
{
	E_ipv4	UMETA(DisplayName = "IPv4"),
	E_ipv6 	UMETA(DisplayName = "IPv6")
};

UENUM(BlueprintType)
enum class ESocketClientTCPMessageWrapping : uint8
{
	E_None 		UMETA(DisplayName = "None"),
	E_String	UMETA(DisplayName = "String"),
	E_Byte		UMETA(DisplayName = "Byte")

};

#ifndef __FileFunctionsSocketClient
#define __FileFunctionsSocketClient
#include "FileFunctionsSocketClient.h"
#endif

#ifndef __SocketClientBPLibrary
#define __SocketClientBPLibrary
#include "SocketClientBPLibrary.h"
#endif

#ifndef __SocketClientPluginTCPClient
#define __SocketClientPluginTCPClient
#include "SocketClientPluginTCPClient.h"
#endif

#ifndef __SocketClientPluginUDPClient
#define __SocketClientPluginUDPClient
#include "SocketClientPluginUDPClient.h"
#endif

class FSocketClientModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static bool isShuttingDown;
};