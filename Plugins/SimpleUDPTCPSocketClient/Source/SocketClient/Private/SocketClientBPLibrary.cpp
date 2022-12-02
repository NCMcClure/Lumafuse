// Copyright 2017-2019 David Romanski (Socke). All Rights Reserved.

#include "SocketClientBPLibrary.h"
#include "SocketClient.h"

USocketClientBPLibrary* USocketClientBPLibrary::socketClientBPLibrary;

USocketClientBPLibrary::USocketClientBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	socketClientBPLibrary = this;

}

/*Delegate functions*/
void USocketClientBPLibrary::socketClientTCPConnectionEventDelegate(const bool success, const FString message, const FString clientConnectionIDP) {}
void USocketClientBPLibrary::receiveTCPMessageEventDelegate(const FString message, const TArray<uint8>& byteArray, const FString clientConnectionIDP) {}
void USocketClientBPLibrary::socketClientUDPConnectionEventDelegate(const bool success, const FString message, const FString clientConnectionID) {}
void USocketClientBPLibrary::receiveUDPMessageEventDelegate(const FString message, const TArray<uint8>& byteArray, const  FString IP, const int32 port,const FString clientConnectionID) {}
void USocketClientBPLibrary::transferFileOverTCPProgressEventDelegate(const FString clientConnectionID, const FString filePath, const float percent, const float mbit, const int64 bytesSend, const int64 fileSize){}
void USocketClientBPLibrary::fileTransferOverTCPInfoEventDelegate(const FString message, const FString clientConnectionID, const FString filePath, const bool success){}
void USocketClientBPLibrary::readBytesFromFileInPartsEventDelegate(const int64 fileSize, const int64 position, const bool end, const TArray<uint8>& byteArray){}


USocketClientBPLibrary::~USocketClientBPLibrary() {

}

USocketClientBPLibrary* USocketClientBPLibrary::getSocketClientTarget() {
	return socketClientBPLibrary;
}



FString USocketClientBPLibrary::getLocalIP() {
	bool canBind = false;
	TSharedRef<FInternetAddr> localIp = USocketClientBPLibrary::getSocketClientTarget()->getSocketSubSystem()->GetLocalHostAddr(*GLog, canBind);

	if (localIp->IsValid()) {
		FString localIP = localIp->ToString(false);
		if (localIP.Equals("127.0.0.1")) {
			UE_LOG(LogTemp, Error, TEXT("Could not detect the local IP."));
			return "0.0.0.0";
		}
		return localIp->ToString(false);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Could not detect the local IP."));
	}
	return "0.0.0.0";
}


void USocketClientBPLibrary::connectSocketClientTCP(FString domain, ESocketClientIPType ipType, int32 port, EReceiveFilterClient receiveFilter, FString& connectionID, bool noPacketDelay/*, bool noPacketBlocking*/) {

	USocketClientPluginTCPClient* tcpClient = NewObject<USocketClientPluginTCPClient>(USocketClientPluginTCPClient::StaticClass());
	connectionID = FGuid::NewGuid().ToString();
	tcpClients.Add(connectionID, tcpClient);
	tcpClient->connect(this, domain, ipType, port, receiveFilter, connectionID, noPacketDelay, false);
}

void USocketClientBPLibrary::socketClientSendTCP(FString connectionID,FString message, TArray<uint8> byteArray, bool addLineBreak) {

	if (connectionID.IsEmpty() || tcpClients.Find(connectionID) == nullptr) {
		//don't send to many error messages. one second = 10000000 ticks
		if (((FDateTime::Now().GetTicks()) - lastErrorMessageTime) >= 10000000) {
			UE_LOG(LogTemp, Error, TEXT("Connection not found (socketClientSendTCPMessage). %s"), *connectionID);
			lastErrorMessageTime = FDateTime::Now().GetTicks();
		}
		return;
	}

	if (message.Len() > 0) {
		if (messageWrapping == ESocketClientTCPMessageWrapping::E_String) {
			message = tcpMessageHeader + message + tcpMessageFooter;
		}
		if (addLineBreak) {
			message.Append("\r\n");
		}
	}

	

	USocketClientPluginTCPClient* tcpClient = *tcpClients.Find(connectionID);
	tcpClient->sendMessage(message, byteArray);

}

void USocketClientBPLibrary::socketClientSendFileOverTCP(FString& connectionID, FString domainOrIP, ESocketClientIPType ipType, int32 port, EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, FString token, FString Aes256bitKey){
	USocketClientPluginTCPClient* tcpClient = NewObject<USocketClientPluginTCPClient>(USocketClientPluginTCPClient::StaticClass());
	connectionID = FGuid::NewGuid().ToString();
	tcpClients.Add(connectionID, tcpClient);
	tcpClient->sendFile(this, connectionID, domainOrIP, ipType, port, directoryType, filePath, token, Aes256bitKey);
}

void USocketClientBPLibrary::socketClientRequestFileOverTCP(FString& connectionID, FString domainOrIP, ESocketClientIPType ipType, int32 port, EFileFunctionsSocketClientDirectoryType directoryType, FString downloadDirectory, bool resume, FString token, FString Aes256bitKey) {
	USocketClientPluginTCPClient* tcpClient = NewObject<USocketClientPluginTCPClient>(USocketClientPluginTCPClient::StaticClass());
	connectionID = FGuid::NewGuid().ToString();
	tcpClients.Add(connectionID, tcpClient);
	tcpClient->requestFile(this, connectionID, domainOrIP, ipType, port, directoryType, downloadDirectory, resume,token, Aes256bitKey);
}



void USocketClientBPLibrary::closeSocketClientConnectionTCP(FString connectionID){
	if (connectionID.IsEmpty()) {
		return;
	}
	if (tcpClients.Find(connectionID) == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Connection not found (closeSocketClientTCPConnection). %s"), *connectionID);
		return;
	}

	USocketClientPluginTCPClient* tcpClient = *tcpClients.Find(connectionID);
	if (tcpClient != nullptr) {
		if (tcpClient->isRun()) {
			tcpClient->closeConnection();
		}
	}
	tcpClients.Remove(connectionID);
	tcpClient = nullptr;
}

void USocketClientBPLibrary::getTCPConnectionByConnectionID(FString connectionID, bool& found, USocketClientPluginTCPClient* &connection){
	if (connectionID.IsEmpty() || tcpClients.Find(connectionID) == nullptr) {
		found = false;
		connection = nullptr;
		return;
	}

	found = true;
	connection = *tcpClients.Find(connectionID);
}

bool USocketClientBPLibrary::isTCPConnected(FString connectionID){
	if (connectionID.IsEmpty() || tcpClients.Find(connectionID) == nullptr) {
		return false;
	}

	return (*tcpClients.Find(connectionID))->isRun();
}

void USocketClientBPLibrary::activateTCPMessageWrappingOnClientPlugin(FString header, FString footer){
	tcpMessageHeader = header;
	tcpMessageFooter = footer;
	messageWrapping = ESocketClientTCPMessageWrapping::E_String;
}

void USocketClientBPLibrary::activateTCPMessageAndBytesWrappingOnClientPlugin() {
	messageWrapping = ESocketClientTCPMessageWrapping::E_Byte;
}

void USocketClientBPLibrary::deactivateTCPMessageWrappingOnClientPlugin(){
	messageWrapping = ESocketClientTCPMessageWrapping::E_None;
}

void USocketClientBPLibrary::getTcpMessageWrapping(FString& header, FString& footer, ESocketClientTCPMessageWrapping& messageWrappingP) {
	header = tcpMessageHeader;
	footer = tcpMessageFooter;
	messageWrappingP = messageWrapping;
}


void USocketClientBPLibrary::socketClientInitUDPReceiver(FString& connectionID, FString domain, ESocketClientIPType ipType, int32 port, EReceiveFilterClient receiveFilter, int32 maxPacketSize) {

	FString key = domain + FString::FromInt(port);
	if (udpClients.Find(key) != nullptr) {
		USocketClientPluginUDPClient* udpClient = *udpClients.Find(key);
		connectionID = udpClient->getConnectionID();
		onsocketClientUDPConnectionEventDelegate.Broadcast(true, "Connection already present:"+domain+":"+ FString::FromInt(port), connectionID);
		return;
	}

	USocketClientPluginUDPClient* udpClient = NewObject<USocketClientPluginUDPClient>(USocketClientPluginUDPClient::StaticClass());
	connectionID = FGuid::NewGuid().ToString();
	udpClients.Add(connectionID, udpClient);
	udpClients.Add(key, udpClient);
	udpClient->init(this, domain, ipType, port, receiveFilter, connectionID, maxPacketSize);
}


void USocketClientBPLibrary::socketClientSendUDP(FString domain, ESocketClientIPType ipType, int32 port, FString message, TArray<uint8> byteArray, bool addLineBreak, FString clientConnectionIDP) {

	if (clientConnectionIDP.IsEmpty() || udpClients.Find(clientConnectionIDP) == nullptr) {
		//don't send to many error messages. one second = 10000000 ticks
		if (((FDateTime::Now().GetTicks()) - lastErrorMessageTime) >= 10000000) {
			UE_LOG(LogTemp, Error, TEXT("Connection not found (socketClientSendUDPMessage). %s"), *clientConnectionIDP);
			lastErrorMessageTime = FDateTime::Now().GetTicks();
		}
		return;
	}

	if (message.Len() > 0 && addLineBreak) {
		message.Append("\r\n");
	}
	USocketClientPluginUDPClient* udpClient = *udpClients.Find(clientConnectionIDP);
	udpClient->sendUDPMessage(domain, ipType, port, message, byteArray);
}



void USocketClientBPLibrary::closeSocketClientConnectionUDP(FString connectionID) {
	if (connectionID.IsEmpty()) {
		return;
	}
	if (udpClients.Find(connectionID) == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Connection not found (closeSocketClientUDPConnection). %s"), *connectionID);
		return;
	}

	USocketClientPluginUDPClient* udpClient = *udpClients.Find(connectionID);
	udpClient->closeUDPConnection();
	udpClients.Remove(connectionID);

	FString key = udpClient->getDomainOrIP() + FString::FromInt(udpClient->getPort());
	udpClients.Remove(key);
	udpClient = nullptr;
}

void USocketClientBPLibrary::getUDPInitializationByConnectionID(FString connectionID, bool& found, USocketClientPluginUDPClient*& connection){
	if (connectionID.IsEmpty() || udpClients.Find(connectionID) == nullptr) {
		found = false;
		connection = nullptr;
		return;
	}

	found = true;
	connection = *udpClients.Find(connectionID);
}

bool USocketClientBPLibrary::isUDPInitialized(FString connectionID){
	if (connectionID.IsEmpty() || udpClients.Find(connectionID) == nullptr) {
		return false;
	}

	return (*udpClients.Find(connectionID))->isRun();
}

void USocketClientBPLibrary::changeSocketPlatform(ESocketPlatformClient platform) {
	USocketClientBPLibrary::getSocketClientTarget()->systemSocketPlatform = platform;
}

FString USocketClientBPLibrary::resolveDomain(FString serverDomainP, ESocketClientIPType ipType) {

	FString* cachedDomainPointer = domainCache.Find(serverDomainP);
	if (cachedDomainPointer != nullptr) {
		return *cachedDomainPointer;
	}

	//is IPv4?
	if (ipType == ESocketClientIPType::E_ipv4) {
		TArray<FString> ipNumbers;
		int32 lineCount = serverDomainP.ParseIntoArray(ipNumbers, TEXT("."), true);
		if (lineCount == 4 && serverDomainP.Len() <= 15 && serverDomainP.Len() >= 7) {
			domainCache.Add(serverDomainP, serverDomainP);
			return serverDomainP;
		}
	}

	//is IPv6? Just a simple check
	if (ipType == ESocketClientIPType::E_ipv6) {
		TArray<FString> ipNumbers;
		int32 lineCount = serverDomainP.ParseIntoArray(ipNumbers, TEXT(":"), true);
		if (lineCount >= 2) {
			domainCache.Add(serverDomainP, serverDomainP);
			return serverDomainP;
		}
	}

	//resolve Domain
	ISocketSubsystem* sSS = USocketClientBPLibrary::getSocketSubSystem();

	auto ResolveInfo = sSS->GetHostByName(TCHAR_TO_ANSI(*serverDomainP));
	while (!ResolveInfo->IsComplete());

	int32 errorCode = ResolveInfo->GetErrorCode();
	if (errorCode == 0) {
		const FInternetAddr* Addr = &ResolveInfo->GetResolvedAddress();
		uint32 OutIP = 0;
		FString adr = Addr->ToString(false);
		domainCache.Add(serverDomainP, adr);
		return adr;
	}
	else {
		if (dnsClient == nullptr)
			dnsClient = NewObject<UDNSClientSocketClient>(UDNSClientSocketClient::StaticClass());
		dnsClient->resolveDomain(sSS, serverDomainP);
		int32 timeout = 1000;
		while (dnsClient->isResloving() && timeout > 0) {
			timeout -= 10;
			FPlatformProcess::Sleep(0.01);
		}
		FString adr = dnsClient->getIP();
		domainCache.Add(serverDomainP, adr);
		return adr;
	}

	return serverDomainP;
}


void USocketClientBPLibrary::getSystemType(ESocketClientSystem& system) {
#if PLATFORM_ANDROID
	system = ESocketClientSystem::Android;
	return;
#endif	
#if PLATFORM_IOS
	system = ESocketClientSystem::IOS;
	return;
#endif	
#if PLATFORM_WINDOWS
	system = ESocketClientSystem::Windows;
	return;
#endif	
#if PLATFORM_LINUX
	system = ESocketClientSystem::Linux;
	return;
#endif	
#if PLATFORM_MAC
	system = ESocketClientSystem::Mac;
	return;
#endif	
}

TArray<uint8> USocketClientBPLibrary::parseHexToBytes(FString hex) {
	TArray<uint8> bytes;

	if (hex.Contains(" ")) {
		hex = hex.Replace(TEXT(" "), TEXT(""));
	}

	if (hex.Len() % 2 != 0) {
		UE_LOG(LogTemp, Error, TEXT("This is not a valid hex string: %s"), *hex);
		return bytes;
	}


	TArray<TCHAR> charArray = hex.GetCharArray();
	for (int32 i = 0; i < (charArray.Num() - 1); i++) {
		if (CheckTCharIsHex(charArray[i]) == false) {
			UE_LOG(LogTemp, Error, TEXT("This is not a valid hex string: %s"), *hex);
			return bytes;
		}
	}


	bytes.AddZeroed(hex.Len() / 2);
	HexToBytes(hex, bytes.GetData());

	return bytes;
}

FString USocketClientBPLibrary::parseHexToString(FString hex) {
	TArray<uint8> bytes = parseHexToBytes(hex);
	bytes.Add(0x00);// null-terminator
	char* Data = (char*)bytes.GetData();
	return FString(UTF8_TO_TCHAR(Data));
}

FString USocketClientBPLibrary::parseBytesToHex(TArray<uint8> bytes) {
	FString hex;
	hex = BytesToHex(bytes.GetData(), bytes.Num());
	return hex;
}

TArray<uint8> USocketClientBPLibrary::parseHexToBytesPure(FString hex) {
	return parseHexToBytes(hex);
}

FString USocketClientBPLibrary::parseHexToStringPure(FString hex) {
	return parseHexToString(hex);
}

FString USocketClientBPLibrary::parseBytesToHexPure(TArray<uint8> bytes) {
	return parseBytesToHex(bytes);
}

void USocketClientBPLibrary::parseBytesToFloat(TArray<uint8> bytes, float& value) {

	if (bytes.Num() != 4) {
		UE_LOG(LogTemp, Error, TEXT("ParseBytesToFloat: Cannot convert bytes to float. Array must contain 4 bytes but has %i bytes."), bytes.Num());
		return;
	}

	FMemory::Memcpy(&value, bytes.GetData(), 4);

}

void USocketClientBPLibrary::parseBytesToInteger(TArray<uint8> bytes, int32& value) {


	if (bytes.Num() != 4) {
		UE_LOG(LogTemp, Error, TEXT("ParseBytesToInteger: Cannot convert bytes to integer. Array must contain 4 bytes but has %i bytes."), bytes.Num());
		return;
	}

	FMemory::Memcpy(&value, bytes.GetData(), 4);
}

void USocketClientBPLibrary::parseBytesToInteger64(TArray<uint8> bytes, int64& value) {


	if (bytes.Num() != 8) {
		UE_LOG(LogTemp, Error, TEXT("ParseBytesToInteger64: Cannot convert bytes to integer. Array must contain 8 bytes but has %i bytes."), bytes.Num());
		return;
	}

	FMemory::Memcpy(&value, bytes.GetData(), 8);
}

void USocketClientBPLibrary::parseBytesToFloatPure(TArray<uint8> bytes, float& value){
	parseBytesToFloat(bytes, value);
}

void USocketClientBPLibrary::parseBytesToIntegerPure(TArray<uint8> bytes, int32& value){
	parseBytesToInteger(bytes, value);
}

void USocketClientBPLibrary::parseBytesToInteger64Pure(TArray<uint8> bytes, int64& value){
	parseBytesToInteger64(bytes, value);
}

void USocketClientBPLibrary::parseBytesToFloatEndian(TArray<uint8> bytes, float& littleEndian, float& bigEndian) {
	littleEndian = 0.f;
	bigEndian = 0.f;

	if (bytes.Num() != 4) {
		UE_LOG(LogTemp, Error, TEXT("ParseBytesToFloat: Cannot convert bytes to float. Array must contain 4 bytes but has %i bytes."), bytes.Num());
		return;
	}

	uint8 littleEndianChar[] = { bytes[0], bytes[1], bytes[2], bytes[3] };
	FMemory::Memcpy(&littleEndian, &littleEndianChar, sizeof(littleEndian));

	uint8 bigEndianChar[] = { bytes[3], bytes[2], bytes[1], bytes[0] };
	FMemory::Memcpy(&bigEndian, &bigEndianChar, sizeof(bigEndian));
}

void USocketClientBPLibrary::parseBytesToIntegerEndian(TArray<uint8> bytes, int32& littleEndian, int32& bigEndian){
	littleEndian = 0;
	bigEndian = 0;

	if (bytes.Num() != 4){
		UE_LOG(LogTemp, Error, TEXT("ParseBytesToInteger: Cannot convert bytes to integer. Array must contain 4 bytes but has %i bytes."), bytes.Num());
		return;
	}
		

	uint8 littleEndianChar[] = { bytes[0], bytes[1], bytes[2], bytes[3] };
	FMemory::Memcpy(&littleEndian, &littleEndianChar, sizeof(littleEndian));

	uint8 bigEndianChar[] = { bytes[3], bytes[2], bytes[1], bytes[0] };
	FMemory::Memcpy(&bigEndian, &bigEndianChar, sizeof(bigEndian));
}

void USocketClientBPLibrary::parseBytesToInteger64Endian(TArray<uint8> bytes, int64& littleEndian, int64& bigEndian) {
	littleEndian = 0;
	bigEndian = 0;

	if (bytes.Num() != 8) {
		UE_LOG(LogTemp, Error, TEXT("ParseBytesToInteger64: Cannot convert bytes to integer. Array must contain 8 bytes but has %i bytes."), bytes.Num());
		return;
	}return;

	uint8 littleEndianChar[] = { bytes[0], bytes[1], bytes[2], bytes[3],bytes[4], bytes[5], bytes[6], bytes[7] };
	FMemory::Memcpy(&littleEndian, &littleEndianChar, sizeof(littleEndian));

	uint8 bigEndianChar[] = { bytes[7], bytes[6], bytes[5], bytes[4], bytes[3], bytes[2], bytes[1], bytes[0] };
	FMemory::Memcpy(&bigEndian, &bigEndianChar, sizeof(bigEndian));
}

void USocketClientBPLibrary::parseFloatToBytes(TArray<uint8>& byteArray, float value, bool switchByteOrder){
	union {
		float tmpVal;
		uint8 tmpArray[4];
	} u;
	u.tmpVal = value;
	byteArray.AddUninitialized(4);

	FMemory::Memcpy(byteArray.GetData(), u.tmpArray, 4);

	if (switchByteOrder) {
		byteArray.SwapMemory(3, 0);
		byteArray.SwapMemory(2, 1);
	}
}

void USocketClientBPLibrary::parseIntegerToBytes(TArray<uint8>& byteArray, int32 value, bool switchByteOrder){
	union {
		int32 tmpVal;
		uint8 tmpArray[4];
	} u;
	u.tmpVal = value;
	byteArray.AddUninitialized(4);

	FMemory::Memcpy(byteArray.GetData(), u.tmpArray, 4);

	if (switchByteOrder) {
		byteArray.SwapMemory(3, 0);
		byteArray.SwapMemory(2, 1);
	}
}

void USocketClientBPLibrary::parseInteger64ToBytes(TArray<uint8>& byteArray, int64 value, bool switchByteOrder){
	union {
		int64 tmpVal;
		uint8 tmpArray[8];
	} u;
	u.tmpVal = value;
	byteArray.AddUninitialized(8);

	FMemory::Memcpy(byteArray.GetData(), u.tmpArray, 8);

	if (switchByteOrder) {
		byteArray.SwapMemory(7, 0);
		byteArray.SwapMemory(6, 1);
		byteArray.SwapMemory(5, 2);
		byteArray.SwapMemory(4, 3);
	}
}

void USocketClientBPLibrary::parseFloatToBytesPure(TArray<uint8>& byteArray, float value, bool switchByteOrder){
	parseFloatToBytes(byteArray, value, switchByteOrder);
}

void USocketClientBPLibrary::parseIntegerToBytesPure(TArray<uint8>& byteArray, int32 value, bool switchByteOrder){
	parseIntegerToBytes(byteArray, value, switchByteOrder);
}

void USocketClientBPLibrary::parseInteger64ToBytesPure(TArray<uint8>& byteArray, int64 value, bool switchByteOrder){
	parseInteger64ToBytes(byteArray, value, switchByteOrder);
}




int32 USocketClientBPLibrary::getUniquePlayerID(APlayerController* playerController) {
	if (playerController == nullptr || playerController->GetLocalPlayer() == nullptr)
		return 0;
	return playerController->GetLocalPlayer()->GetUniqueID();
}

FString USocketClientBPLibrary::getRandomID(){
	return FGuid::NewGuid().ToString();
}


ISocketSubsystem* USocketClientBPLibrary::getSocketSubSystem() {
	switch (USocketClientBPLibrary::getSocketClientTarget()->systemSocketPlatform)
	{
	case ESocketPlatformClient::E_SSC_SYSTEM:
		return ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	case ESocketPlatformClient::E_SSC_WINDOWS:
		return ISocketSubsystem::Get(FName(TEXT("WINDOWS")));
	case ESocketPlatformClient::E_SSC_MAC:
		return ISocketSubsystem::Get(FName(TEXT("MAC")));
	case ESocketPlatformClient::E_SSC_IOS:
		return ISocketSubsystem::Get(FName(TEXT("IOS")));
	case ESocketPlatformClient::E_SSC_UNIX:
		return ISocketSubsystem::Get(FName(TEXT("UNIX")));
	case ESocketPlatformClient::E_SSC_ANDROID:
		return ISocketSubsystem::Get(FName(TEXT("ANDROID")));
	case ESocketPlatformClient::E_SSC_PS4:
		return ISocketSubsystem::Get(FName(TEXT("PS4")));
	case ESocketPlatformClient::E_SSC_XBOXONE:
		return ISocketSubsystem::Get(FName(TEXT("XBOXONE")));
	case ESocketPlatformClient::E_SSC_HTML5:
		return ISocketSubsystem::Get(FName(TEXT("HTML5")));
	case ESocketPlatformClient::E_SSC_SWITCH:
		return ISocketSubsystem::Get(FName(TEXT("SWITCH")));
	case ESocketPlatformClient::E_SSC_DEFAULT:
		return ISocketSubsystem::Get();
	default:
		return ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	}
}


