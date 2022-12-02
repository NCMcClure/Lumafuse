// Copyright 2017-2019 David Romanski (Socke). All Rights Reserved.

#include "SocketClientPluginUDPClient.h"


USocketClientPluginUDPClient::USocketClientPluginUDPClient(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	this->AddToRoot();
}



void USocketClientPluginUDPClient::socketClientUDPConnectionEventDelegate(const bool success, const FString message, const FString clientConnectionIDP){}
void USocketClientPluginUDPClient::receiveUDPMessageEventDelegate(const FString message, const TArray<uint8>& byteArray, const FString IPP, const int32 portP, const FString clientConnectionIDP) {}

void USocketClientPluginUDPClient::init(USocketClientBPLibrary* socketClientLibP, FString domainOrIPP, ESocketClientIPType ipType, int32 portP, EReceiveFilterClient receiveFilterP, FString connectionIDP,
	int32 maxPacketSizeP) {
	socketClientBPLibrary = socketClientLibP;
	receiveFilter = receiveFilterP;
	connectionID = connectionIDP;
	domainOrIP = domainOrIPP;
	port = portP;
	maxPacketSize = maxPacketSizeP;
	if (maxPacketSize < 1 || maxPacketSize > 65507)
		maxPacketSize = 65507;

	UDPThread = new FServerUDPConnectionThread(this, socketClientLibP, domainOrIPP, portP, ipType);
}


void USocketClientPluginUDPClient::sendUDPMessage(FString domainOrIPP, ESocketClientIPType ipType, int32 portP, FString message, TArray<uint8> byteArray){
	if (UDPSendThread != nullptr) {
		UDPSendThread->addData(message, byteArray, domainOrIPP, portP, ipType);
	}
}

void USocketClientPluginUDPClient::closeUDPConnection() {

	if (udpSocketReceiver != nullptr) {
		udpSocketReceiver->Stop();
		udpSocketReceiver = nullptr;
	}

	setRun(false);
	UDPThread = nullptr;
	if (UDPSendThread != nullptr) {
		UDPSendThread->pauseThread(false);
	}

	UDPSendThread = nullptr;
}


void USocketClientPluginUDPClient::UDPReceiver(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt) {

	if (FSocketClientModule::isShuttingDown)
		return;

	TSharedPtr<FInternetAddr> peerAddr = EndPt.ToInternetAddr();
	FString ipGlobal = peerAddr->ToString(false);
	int32 portGlobal = peerAddr->GetPort();

	TArray<uint8> byteArray;
	if (receiveFilter == EReceiveFilterClient::E_SAB || receiveFilter == EReceiveFilterClient::E_B) {
		byteArray.Append(ArrayReaderPtr->GetData(), ArrayReaderPtr->Num());
	}

	FString recvMessage;
	if (receiveFilter == EReceiveFilterClient::E_SAB || receiveFilter == EReceiveFilterClient::E_S) {
		ArrayReaderPtr->Add(0x00);// null-terminator
		char* Data = (char*)ArrayReaderPtr->GetData();
		recvMessage = FString(UTF8_TO_TCHAR(Data));
	}

	//switch to gamethread
	USocketClientBPLibrary* socketClientBPLibraryGlobal = socketClientBPLibrary;
	USocketClientPluginUDPClient* udpClientGlobal = this;
	FString clientConnectionIDGlobal = connectionID;
	AsyncTask(ENamedThreads::GameThread, [udpClientGlobal, recvMessage, byteArray, ipGlobal, portGlobal, socketClientBPLibraryGlobal, clientConnectionIDGlobal]() {
		if (FSocketClientModule::isShuttingDown)
			return;
		socketClientBPLibraryGlobal->onreceiveUDPMessageEventDelegate.Broadcast(recvMessage, byteArray, ipGlobal, portGlobal, clientConnectionIDGlobal);
		udpClientGlobal->onreceiveUDPMessageEventDelegate.Broadcast(recvMessage, byteArray, ipGlobal, portGlobal, clientConnectionIDGlobal);
	});
}

bool USocketClientPluginUDPClient::isRun(){
	return run;
}

void USocketClientPluginUDPClient::setRun(bool runP){
	run = runP;
}

FSocket* USocketClientPluginUDPClient::getSocket(){
	return socket;
}

void USocketClientPluginUDPClient::setSocket(FSocket* socketP){
	socket = socketP;
}

void USocketClientPluginUDPClient::setUDPSocketReceiver(FUdpSocketReceiver* udpSocketReceiverP){
	udpSocketReceiver = udpSocketReceiverP;
}

FString USocketClientPluginUDPClient::getIP(){
	return domainOrIP;
}

void USocketClientPluginUDPClient::setIP(FString ipP){
	domainOrIP = ipP;
}

int32 USocketClientPluginUDPClient::getPort(){
	return port;
}

FString USocketClientPluginUDPClient::getDomainOrIP(){
	return domainOrIP;
}

FString USocketClientPluginUDPClient::getConnectionID(){
	return connectionID;
}

void USocketClientPluginUDPClient::setUDPSendThread(FServerUDPSendMessageThread* udpSendThreadP){
	UDPSendThread = udpSendThreadP;
}

int32 USocketClientPluginUDPClient::getMaxPacketSize()
{
	return maxPacketSize;
}
