// Copyright 2017-2019 David Romanski (Socke). All Rights Reserved.

#include "SocketClientPluginTCPClient.h"


USocketClientPluginTCPClient::USocketClientPluginTCPClient(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	//This prevents the garbage collector from killingand deleting the class from RAM.
	this->AddToRoot();
}


void USocketClientPluginTCPClient::socketClientTCPConnectionEventDelegate(const bool success, const FString message, const FString clientConnectionID){}
void USocketClientPluginTCPClient::receiveTCPMessageEventDelegate(const FString message, const TArray<uint8>& byteArray, const FString clientConnectionID){}
void USocketClientPluginTCPClient::transferFileOverTCPProgressEventDelegate(const FString clientConnectionID, const FString filePathP, const float percent, const float mbit, const int64 bytesTransferred, const int64 fileSize) {}
void USocketClientPluginTCPClient::fileTransferOverTCPInfoEventDelegate(const FString message, const FString clientConnectionID, const FString filePathP, const bool success) {}

void USocketClientPluginTCPClient::connect(USocketClientBPLibrary* mainLibP, FString domainOrIP, ESocketClientIPType ipType, int32 port, EReceiveFilterClient receiveFilter, FString connectionIDP, bool noPacketDelay, bool noPacketBlocking){
	mainLib = mainLibP;
	connectionID = connectionIDP;
	tcpConnectionThread = new FServerConnectionThread(mainLib, connectionID, receiveFilter, domainOrIP, ipType, port,this,noPacketDelay,noPacketBlocking);
}

void USocketClientPluginTCPClient::sendMessage(FString message, TArray<uint8> byteArray){
	if (run && tcpSendThread != nullptr) {
		tcpSendThread->sendMessage(message, byteArray);
	}
}

void USocketClientPluginTCPClient::sendFile(USocketClientBPLibrary* mainLibP, FString connectionIDP, FString domainOrIP, ESocketClientIPType ipType, int32 port, EFileFunctionsSocketClientDirectoryType directoryType, FString filePathP, FString token, FString Aes256bitKey){
	mainLib = mainLibP;
	connectionID = connectionIDP;
	aesKey = Aes256bitKey;
	fileToken = token;
	sendOrReceive = 0;
	filePath = UFileFunctionsSocketClient::getCleanDirectory(directoryType, filePathP);
	tcpFileConnectionThread = new FServerFileConnectionThread(mainLib, connectionID, domainOrIP, ipType, port, this);
}

void USocketClientPluginTCPClient::requestFile(USocketClientBPLibrary* mainLibP, FString connectionIDP, FString domainOrIP, ESocketClientIPType ipType, int32 port, EFileFunctionsSocketClientDirectoryType directoryType, FString filePathP, bool resumeP, FString token, FString Aes256bitKey) {
	mainLib = mainLibP;
	connectionID = connectionIDP;
	aesKey = Aes256bitKey;
	fileToken = token;
	sendOrReceive = 1;
	resume = resumeP;
	filePath = UFileFunctionsSocketClient::getCleanDirectory(directoryType, filePathP);
	tcpFileConnectionThread = new FServerFileConnectionThread(mainLib, connectionID, domainOrIP, ipType, port, this);
}

void USocketClientPluginTCPClient::closeConnection(){

	if (tcpFileConnectionThread != nullptr) {
		tcpFileConnectionThread->triggerFileTransferOverTCPInfoEvent("Data transfer aborted.", connectionID, filePath, false, mainLib, this);
	}

	setRun(false);
	tcpConnectionThread = nullptr;
	tcpSendThread = nullptr;
	mainLib = nullptr;
	tcpFileConnectionThread = nullptr;
	fileSendThread = nullptr;
}

bool USocketClientPluginTCPClient::isRun(){
	return run;
}

void USocketClientPluginTCPClient::setRun(bool runP) {
	run = runP;
}

FString USocketClientPluginTCPClient::getConnectionID(){
	return connectionID;
}

FString USocketClientPluginTCPClient::getAesKey(){
	return aesKey;
}

FString USocketClientPluginTCPClient::getFileToken(){
	return fileToken;
}

FString USocketClientPluginTCPClient::getFilePath() {
	return filePath;
}

void USocketClientPluginTCPClient::setSocket(FSocket* socketP){
	socket = socketP;
}

FSocket* USocketClientPluginTCPClient::getSocket(){
	return socket;
}

void USocketClientPluginTCPClient::createSendThread(){
	tcpSendThread = new FSendDataToServerThread(mainLib, this, connectionID);
}

void USocketClientPluginTCPClient::createFileSendThread(int64 startPosition){
	fileSendThread = new FSendFileToServerThread(mainLib, this, connectionID, filePath, startPosition);
}

FSendFileToServerThread* USocketClientPluginTCPClient::getFileSendThread() {
	return fileSendThread;
}

FString USocketClientPluginTCPClient::encryptMessage(FString message) {
	return UFileFunctionsSocketClient::encryptMessageWithAES(message, aesKey);
}

FString USocketClientPluginTCPClient::decryptMessage(FString message) {
	return UFileFunctionsSocketClient::decryptMessageWithAES(message, aesKey);
}

bool USocketClientPluginTCPClient::isSendFile(){
	return (sendOrReceive == 0);
}

bool USocketClientPluginTCPClient::isReceiveFile(){
	return (sendOrReceive == 1);
}

bool USocketClientPluginTCPClient::hasResume(){
	return resume;
}

void USocketClientPluginTCPClient::deleteFile(FString filePathP){
	UFileFunctionsSocketClient::deleteFileAbsolutePath(filePathP);
}

void USocketClientPluginTCPClient::getMD5FromFileAbsolutePath(FString filePathP, bool& success, FString& MD5) {
	UFileFunctionsSocketClient::getMD5FromFileAbsolutePath(filePathP, success, MD5);
}

int64 USocketClientPluginTCPClient::fileSize(FString filePathP) {
	return UFileFunctionsSocketClient::fileSizeAbsolutePath(filePathP);
}

USocketClientBPLibrary* USocketClientPluginTCPClient::getMainLib() {
	return mainLib;
}

FString USocketClientPluginTCPClient::int64ToString(int64 num) {
	return UFileFunctionsSocketClient::int64ToString(num);
}
