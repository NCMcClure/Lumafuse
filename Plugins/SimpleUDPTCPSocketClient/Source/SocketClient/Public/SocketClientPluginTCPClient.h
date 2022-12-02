// Copyright 2017-2019 David Romanski (Socke). All Rights Reserved.
#pragma once

#include "SocketClient.h"
#include "SocketClientPluginTCPClient.generated.h"


class USocketClientBPLibrary;
class FServerConnectionThread;
class FSendDataToServerThread;
class FServerFileConnectionThread;
class FSendFileToServerThread;

UCLASS(Blueprintable, BlueprintType)
class SOCKETCLIENT_API  USocketClientPluginTCPClient : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	//Delegates
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FsocketClientTCPConnectionEventDelegate, bool, success, FString, message, FString, clientConnectionID);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FreceiveTCPMessageEventDelegate, FString, message, const TArray<uint8>&, byteArray, FString, clientConnectionID);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FtransferFileOverTCPProgressEventDelegate, FString, clientConnectionID, FString, filePath, float, percent, float, mbit, int64, bytesTransferred, int64, fileSize);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FfileTransferOverTCPInfoEventDelegate, FString, message, FString, clientConnectionID, FString, filePath, bool, success);

	UFUNCTION()
		void socketClientTCPConnectionEventDelegate(const bool success, const FString message, const FString clientConnectionID);
	UPROPERTY(BlueprintAssignable, Category = "SocketClient|TCP|Events|ConnectionInfo")
		FsocketClientTCPConnectionEventDelegate onsocketClientTCPConnectionEventDelegate;
	UFUNCTION()
		void receiveTCPMessageEventDelegate(const FString message, const TArray<uint8>& byteArray, const FString clientConnectionID);
	UPROPERTY(BlueprintAssignable, Category = "SocketClient|TCP|Events|ReceiveMessage")
		FreceiveTCPMessageEventDelegate onreceiveTCPMessageEventDelegate;
	UFUNCTION()
		void transferFileOverTCPProgressEventDelegate(const FString clientConnectionID, const FString filePath, const float percent, const float mbit, const int64 bytesTransferred, const int64 fileSize);
	UPROPERTY(BlueprintAssignable, Category = "SocketClient|TCP|Events|File|transferFileOverTCPProgress")
		FtransferFileOverTCPProgressEventDelegate ontransferFileOverTCPProgressEventDelegate;
	UFUNCTION()
		void fileTransferOverTCPInfoEventDelegate(const FString message, const FString clientConnectionID, const FString filePath, const bool success);
	UPROPERTY(BlueprintAssignable, Category = "SocketClient|TCP|Events|File|FileTransferOverTCPInfo")
		FfileTransferOverTCPInfoEventDelegate onfileTransferOverTCPInfoEventDelegate;

	void connect(USocketClientBPLibrary* mainLib, FString domainOrIP, ESocketClientIPType ipType, int32 port, EReceiveFilterClient receiveFilter, FString connectionID, bool noPacketDelay = false, bool noPacketBlocking = false);
	void sendMessage(FString message, TArray<uint8> byteArray);
	void sendFile(USocketClientBPLibrary* mainLib, FString connectionID, FString domainOrIP, ESocketClientIPType ipType, int32 port, EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, FString token, FString Aes256bitKey);
	void requestFile(USocketClientBPLibrary* mainLib, FString connectionID, FString domainOrIP, ESocketClientIPType ipType, int32 port, EFileFunctionsSocketClientDirectoryType directoryType, FString filePath,bool resume, FString token, FString Aes256bitKey);
	
	void closeConnection();

	bool isRun();
	void setRun(bool runP);
	FString getConnectionID();
	FString getAesKey();
	FString getFileToken();
	FString getFilePath();

	void setSocket(FSocket* socket);
	FSocket* getSocket();
	USocketClientBPLibrary* getMainLib();

	void createSendThread();
	void createFileSendThread(int64 startPosition);
	FString encryptMessage(FString message);
	FString decryptMessage(FString message);
	bool isSendFile();
	bool isReceiveFile();
	bool hasResume();
	void deleteFile(FString filePathP);
	void getMD5FromFileAbsolutePath(FString filePath, bool& success, FString& MD5);
	int64 fileSize(FString filePath);
	FString int64ToString(int64 num);
	FSendFileToServerThread* getFileSendThread();
	

private:

	bool run = false;
	bool resume = false;
	int32 sendOrReceive = -1; //0 == send, 1 == receive;
	FString connectionID = FString();
	FString aesKey = FString();
	FString fileToken = FString();
	FString filePath = FString();


	FSocket* socket = nullptr;

	FServerConnectionThread* tcpConnectionThread = nullptr;
	FSendDataToServerThread* tcpSendThread = nullptr;
	FServerFileConnectionThread* tcpFileConnectionThread = nullptr;
	FSendFileToServerThread* fileSendThread = nullptr;
	USocketClientBPLibrary* mainLib = nullptr;

};

/* asynchronous Thread*/
class SOCKETCLIENT_API FServerConnectionThread : public FRunnable {

public:

	FServerConnectionThread(USocketClientBPLibrary* socketClientP, FString clientConnectionIDP, EReceiveFilterClient receiveFilterP, FString ipOrDomainP, ESocketClientIPType ipTypeP, int32 portP, USocketClientPluginTCPClient* tcpClientP, bool noPacketDelayP, bool noPacketBlockingP) :
		socketClient(socketClientP),
		clientConnectionID(clientConnectionIDP),
		receiveFilter(receiveFilterP),
		ipOrDomain(ipOrDomainP),
		ipType(ipTypeP),
		port(portP),
		tcpClient(tcpClientP),
		noPacketDelay(noPacketDelayP),
		noPacketBlocking(noPacketBlockingP){
		FString threadName = "FServerConnectionThread" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {
		//UE_LOG(LogTemp, Display, TEXT("DoWork:%s"),*(FDateTime::Now()).ToString());
		FString ip = socketClient->resolveDomain(ipOrDomain, ipType);
		int32 portGlobal = port;
		FString clientConnectionIDGlobal = clientConnectionID;
		USocketClientBPLibrary* socketClientGlobal = socketClient;
		USocketClientPluginTCPClient* tcpClientGlobal = tcpClient;

		//message wrapping
		FString tcpMessageHeader;
		FString tcpMessageFooter;
		ESocketClientTCPMessageWrapping messageWrapping = ESocketClientTCPMessageWrapping::E_None;

		socketClient->getTcpMessageWrapping(tcpMessageHeader, tcpMessageFooter, messageWrapping);

		FString tcpMessageFooterLineBreak = tcpMessageFooter+"\r\n";
		FString tcpMessageFooterLineBreak2 = tcpMessageFooter + "\r";;


		//UE_LOG(LogTemp, Warning, TEXT("Tread:%s:%i"),*ip, port);
		ISocketSubsystem* sSS = USocketClientBPLibrary::getSocketSubSystem();
		if (sSS == nullptr) {
			AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, socketClientGlobal, tcpClientGlobal]() {
				if (socketClientGlobal != nullptr)
					socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(1). SocketSubSystem does not exist." + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				if (tcpClientGlobal != nullptr)
					tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(1). SocketSubSystem does not exist." + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				});
			return 0;
		}
		TSharedRef<FInternetAddr> addr = sSS->CreateInternetAddr();
		bool bIsValid;
		addr->SetIp(*ip, bIsValid);
		addr->SetPort(port);

		if (bIsValid) {
			// create the socket
			FSocket* socket = sSS->CreateSocket(NAME_Stream, TEXT("socketClient"), addr->GetProtocolType());
			tcpClient->setSocket(socket);


			//socket options
			if (socket != nullptr) {
				socket->SetNoDelay(noPacketDelay);
				socket->SetNonBlocking(noPacketBlocking);
			}

			// try to connect to the server
			if (socket == nullptr || socket->Connect(*addr) == false) {
				const TCHAR* socketErr = sSS->GetSocketError(SE_GET_LAST_ERROR_CODE);
				AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, socketClientGlobal, tcpClientGlobal, socketErr]() {
					if (socketClientGlobal != nullptr)
						socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(2):" + FString(socketErr) + "|" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					if (tcpClientGlobal != nullptr)
						tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(2):" + FString(socketErr) + "|" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					});
			}
			else {
				AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal]() {
					if (socketClientGlobal != nullptr)
						socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(true, "Connection successful:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					if (tcpClientGlobal != nullptr)
						tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(true, "Connection successful:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					});

				
				tcpClient->setRun(true);
				tcpClient->createSendThread();
				int64 ticks1;
				int64 ticks2;
				TArray<uint8> byteDataArray;
				TArray<uint8> byteDataArrayCache;
				FString mainMessage;
				bool inCollectMessageStatus = false;	
				int32 lastDataLengthFromHeader = 0;


				uint32 dataSize;
				while (socket != nullptr && tcpClient->isRun()) {

					//ESocketConnectionState::SCS_Connected does not work https://issues.unrealengine.com/issue/UE-27542
					//Compare ticks is a workaround to get a disconnect. clientSocket->Wait() stop working after disconnect. (Another bug?)
					//If it doesn't wait any longer, ticks1 and ticks2 should be the same == disconnect.
					ticks1 = FDateTime::Now().GetTicks();
					socket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan::FromSeconds(0.1));
					ticks2 = FDateTime::Now().GetTicks();

					bool hasData = socket->HasPendingData(dataSize);
					if (!hasData && ticks1 == ticks2) {
						UE_LOG(LogTemp, Display, TEXT("TCP connection broken. End Loop"));
						break;
					}

					if (hasData) {
						TArray<uint8> dataFromSocket;
						dataFromSocket.SetNumUninitialized(dataSize);
						int32 BytesRead = 0;
						if (socket->Recv(dataFromSocket.GetData(), dataFromSocket.Num(), BytesRead)) {

							switch (messageWrapping)
							{
							case ESocketClientTCPMessageWrapping::E_None:
								triggerMessageEvent(dataFromSocket, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal);
								break;
							case ESocketClientTCPMessageWrapping::E_String:
								if (receiveFilter == EReceiveFilterClient::E_SAB || receiveFilter == EReceiveFilterClient::E_S) {
									dataFromSocket.Add(0x00);// null-terminator
									char* Data = (char*)dataFromSocket.GetData();

									FString recvMessage = FString(UTF8_TO_TCHAR(Data));
									if (recvMessage.StartsWith(tcpMessageHeader)) {
										inCollectMessageStatus = true;
										recvMessage.RemoveFromStart(tcpMessageHeader);
									}
									if (recvMessage.EndsWith(tcpMessageFooter) || recvMessage.EndsWith(tcpMessageFooterLineBreak) || recvMessage.EndsWith(tcpMessageFooterLineBreak2)) {
										inCollectMessageStatus = false;
										if (!recvMessage.RemoveFromEnd(tcpMessageFooter)) {
											if (!recvMessage.RemoveFromEnd(tcpMessageFooterLineBreak)) {
												if (recvMessage.RemoveFromEnd(tcpMessageFooterLineBreak2)) {
													recvMessage.Append("\r");
												}
											}
											else {
												recvMessage.Append("\r\n");
											}
										}

										//splitt merged messages
										if (recvMessage.Contains(tcpMessageHeader)) {
											TArray<FString> lines;
											int32 lineCount = recvMessage.ParseIntoArray(lines, *tcpMessageHeader, true);
											for (int32 i = 0; i < lineCount; i++) {
												mainMessage = lines[i];
												if (mainMessage.EndsWith(tcpMessageFooter) || mainMessage.EndsWith(tcpMessageFooterLineBreak) || mainMessage.EndsWith(tcpMessageFooterLineBreak2)) {
													if (!mainMessage.RemoveFromEnd(tcpMessageFooter)) {
														if (!mainMessage.RemoveFromEnd(tcpMessageFooterLineBreak)) {
															if (mainMessage.RemoveFromEnd(tcpMessageFooterLineBreak2)) {
																mainMessage.Append("\r");
															}
														}
														else {
															mainMessage.Append("\r\n");
														}
													}
												}

												//switch to gamethread
												AsyncTask(ENamedThreads::GameThread, [mainMessage, byteDataArray, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal]() {
													if (socketClientGlobal != nullptr)
														socketClientGlobal->onreceiveTCPMessageEventDelegate.Broadcast(mainMessage, byteDataArray, clientConnectionIDGlobal);
													if (tcpClientGlobal != nullptr)
														tcpClientGlobal->onreceiveTCPMessageEventDelegate.Broadcast(mainMessage, byteDataArray, clientConnectionIDGlobal);
												});

												dataFromSocket.Empty();
												mainMessage.Empty();
											}
											continue;
										}
										else {
											mainMessage.Append(recvMessage);
										}


									}
									if (inCollectMessageStatus) {
										mainMessage.Append(recvMessage);
										continue;
									}
									if (mainMessage.IsEmpty()) {
										continue;
									}

								}
								break;
							case ESocketClientTCPMessageWrapping::E_Byte:
								
								if (lastDataLengthFromHeader == 0 && dataFromSocket.Num() >= 5) {

									readDataLength(dataFromSocket, lastDataLengthFromHeader);

									if (dataFromSocket.Num() == 5) {
										dataFromSocket.Empty();
										continue;
									}

									byteDataArrayCache.Append(dataFromSocket.GetData() + 5, dataFromSocket.Num() - 5);
									dataFromSocket.Empty();
								}
								else {
									byteDataArrayCache.Append(dataFromSocket.GetData(), dataFromSocket.Num());
								}

								int32 maxLoops = 1000;//to prevent endless loop

								while (byteDataArrayCache.Num() > 0 && byteDataArrayCache.Num() >= lastDataLengthFromHeader && maxLoops > 0) {
									maxLoops--;

									byteDataArray.Append(byteDataArrayCache.GetData(), lastDataLengthFromHeader);
									byteDataArrayCache.RemoveAt(0, lastDataLengthFromHeader, true);


									triggerMessageEvent(byteDataArray, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal);
									//UE_LOG(LogTemp, Display, TEXT("%s"), *mainMessage);
																		
									byteDataArray.Empty();

									if (byteDataArrayCache.Num() == 0) {
										lastDataLengthFromHeader = 0;
										break;
									}

									if (byteDataArrayCache.Num() > 5) {
										readDataLength(byteDataArrayCache, lastDataLengthFromHeader);
										byteDataArrayCache.RemoveAt(0, 5, true);
									}

								}
								continue;
								break;

							}

							//triggerMessageEvent(mainMessage, byteDataArray, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal);

						}
						mainMessage.Empty();
						byteDataArray.Empty();
						dataFromSocket.Empty();
					}
				}


				AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal]() {
					if (socketClientGlobal != nullptr) {
						socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection close:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					if (tcpClientGlobal != nullptr)
						tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection close:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					}
				});
			}

			//wait for game thread
			//FPlatformProcess::Sleep(1);


			USocketClientBPLibrary::getSocketClientTarget()->closeSocketClientConnectionTCP(clientConnectionID);


			
			tcpClient->setRun(false);
			if (socket != nullptr) {
				socket->Close();
				sSS->DestroySocket(socket);
				socket = nullptr;
			}
			thread = nullptr;
		}
		else {
			AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal]() {
				if (socketClientGlobal != nullptr)
					socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(3). IP not valid:" + ip+ ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				if (tcpClientGlobal != nullptr)
					tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(3). IP not valid:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				});
		}

		return 0;
	}

	void triggerMessageEvent(TArray<uint8>& byteDataArray, FString&  clientConnectionIDGlobal, USocketClientPluginTCPClient* tcpClientGlobal, USocketClientBPLibrary* socketClientGlobal) {


		//if (receiveFilter == EReceiveFilterClient::E_SAB || receiveFilter == EReceiveFilterClient::E_B) {
		//	byteDataArray.Append(dataFromSocket.GetData(), dataFromSocket.Num());
		//}

		FString mainMessage = FString();
		if (receiveFilter == EReceiveFilterClient::E_SAB || receiveFilter == EReceiveFilterClient::E_S) {
			byteDataArray.Add(0x00);// null-terminator
			char* Data = (char*)byteDataArray.GetData();
			mainMessage = FString(UTF8_TO_TCHAR(Data));
			if (receiveFilter == EReceiveFilterClient::E_S) {
				byteDataArray.Empty();
			}
		}

		//switch to gamethread
		AsyncTask(ENamedThreads::GameThread, [mainMessage, byteDataArray, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal]() {
			if (socketClientGlobal != nullptr)
				socketClientGlobal->onreceiveTCPMessageEventDelegate.Broadcast(mainMessage, byteDataArray, clientConnectionIDGlobal);
			if (tcpClientGlobal != nullptr)
				tcpClientGlobal->onreceiveTCPMessageEventDelegate.Broadcast(mainMessage, byteDataArray, clientConnectionIDGlobal);
		});
		mainMessage.Empty();
	}

	void readDataLength(TArray<uint8>& byteDataArray, int32& byteLenght) {
		if (FGenericPlatformProperties::IsLittleEndian() && byteDataArray[0] == 0x00) {
			FMemory::Memcpy(&byteLenght, byteDataArray.GetData() + 1, 4);
		}
		else {
			//endian fits not. swap bytes that contains the length
			byteDataArray.SwapMemory(1, 4);
			byteDataArray.SwapMemory(2, 3);
			FMemory::Memcpy(&byteLenght, byteDataArray.GetData() + 1, 4);
		}
	}


protected:
	USocketClientBPLibrary* socketClient = nullptr;
	//USocketClientBPLibrary*		oldClient;
	FString						clientConnectionID;
	FString						originalIP;
	EReceiveFilterClient		receiveFilter;
	FString ipOrDomain;
	ESocketClientIPType ipType;
	int32 port;
	USocketClientPluginTCPClient* tcpClient = nullptr;
	bool noPacketDelay = false;
	bool noPacketBlocking = false;


	FRunnableThread* thread = nullptr;
};


/* asynchronous Thread*/
class SOCKETCLIENT_API FSendDataToServerThread : public FRunnable {

public:

	FSendDataToServerThread(USocketClientBPLibrary* socketClientLibP, USocketClientPluginTCPClient* tcpClientP, FString clientConnectionIDP) :
		socketClientLib(socketClientLibP),
		tcpClient(tcpClientP),
		clientConnectionID(clientConnectionIDP) {
		FString threadName = "FSendDataToServerThread" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {

		if (tcpClient == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("Class is not initialized."));
			return 0;
		}

		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("tcp socket 1"));*/

		FString clientConnectionIDGlobal = clientConnectionID;
		USocketClientBPLibrary* socketClientLibGlobal = socketClientLib;

		//message wrapping
		FString tcpMessageHeader;
		FString tcpMessageFooter;
		ESocketClientTCPMessageWrapping messageWrapping = ESocketClientTCPMessageWrapping::E_None;

		socketClientLib->getTcpMessageWrapping(tcpMessageHeader, tcpMessageFooter, messageWrapping);

		// get the socket
		FSocket* socket = tcpClient->getSocket();

		while (tcpClient->isRun()) {

			// try to connect to the server
			if (socket == NULL || socket == nullptr) {
				UE_LOG(LogTemp, Error, TEXT("Connection not exist."));
				//AsyncTask(ENamedThreads::GameThread, [clientConnectionIDGlobal, socketClientLibGlobal]() {
				//	if (socketClientLibGlobal != nullptr) {
				//		socketClientLibGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection not exist:" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				//		socketClientLibGlobal->closeSocketClientConnection();
				//	}
				//});
				break;
			}

			if (socket != nullptr && socket->GetConnectionState() == ESocketConnectionState::SCS_Connected) {
				while (messageQueue.IsEmpty() == false) {
					FString m;
					messageQueue.Dequeue(m);
					FTCHARToUTF8 Convert(*m);
					int32 sent = 0;

					if (messageWrapping == ESocketClientTCPMessageWrapping::E_Byte) {
						TArray<uint8> byteCache;
						if (FGenericPlatformProperties::IsLittleEndian()) {
							byteCache.Add(0x00);
						}
						else {
							byteCache.Add(0x01);
						}
						byteCache.SetNum(5);
						int32 dataLength = Convert.Length();
						FMemory::Memcpy(byteCache.GetData()+1,&dataLength,4);
						byteCache.Append((uint8*)Convert.Get(), Convert.Length());
						socket->Send(byteCache.GetData(), byteCache.Num(), sent);
						byteCache.Empty();
					}
					else {
						socket->Send((uint8*)Convert.Get(), Convert.Length(), sent);
					}
				}

				while (byteArrayQueue.IsEmpty() == false) {
					TArray<uint8> ba;
					byteArrayQueue.Dequeue(ba);
					int32 sent = 0;
					if (messageWrapping == ESocketClientTCPMessageWrapping::E_Byte) {
						TArray<uint8> byteCache;
						if (FGenericPlatformProperties::IsLittleEndian()) {
							byteCache.Add(0x00);
						}
						else {
							byteCache.Add(0x01);
						}
						byteCache.SetNum(5);
						int32 dataLength = ba.Num();
						FMemory::Memcpy(byteCache.GetData() + 1, &dataLength, 4);
						byteCache.Append(ba.GetData(), ba.Num());
						socket->Send(byteCache.GetData(), byteCache.Num(), sent);
					}
					else {
						socket->Send(ba.GetData(), ba.Num(), sent);
					}
					ba.Empty();
				}

			}

			if (tcpClient->isRun()) {
				pauseThread(true);
				//workaround. suspend do not work on all platforms. lets sleep
				while (paused && tcpClient->isRun()) {
					FPlatformProcess::Sleep(0.01);
				}
			}
		}
		thread = nullptr;
		return 0;
	}


	void sendMessage(FString messageP, TArray<uint8> byteArrayP) {
		if (messageP.Len() > 0)
			messageQueue.Enqueue(messageP);
		if (byteArrayP.Num() > 0)
			byteArrayQueue.Enqueue(byteArrayP);
		pauseThread(false);
	}


	void pauseThread(bool pause) {
		paused = pause;
		if (thread != nullptr)
			thread->Suspend(pause);
	}


protected:
	TQueue<FString> messageQueue;
	TQueue<TArray<uint8>> byteArrayQueue;
	USocketClientBPLibrary* socketClientLib;
	USocketClientPluginTCPClient* tcpClient = nullptr;
	FString clientConnectionID;
	FRunnableThread* thread = nullptr;
	bool					run = true;
	bool					paused = false;
	bool blah = true;
};




/*File Threads*/


/* asynchronous Thread*/
class SOCKETCLIENT_API FServerFileConnectionThread : public FRunnable {

public:

	FServerFileConnectionThread(USocketClientBPLibrary* socketClientP, FString clientConnectionIDP, FString ipOrDomainP, ESocketClientIPType ipTypeP, int32 portP, USocketClientPluginTCPClient* tcpClientP) :
		socketClient(socketClientP),
		clientConnectionID(clientConnectionIDP),
		ipOrDomain(ipOrDomainP),
		ipType(ipTypeP),
		port(portP),
		tcpClient(tcpClientP) {
		FString threadName = "FServerFileConnectionThread" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {
		//UE_LOG(LogTemp, Display, TEXT("DoWork:%s"),*(FDateTime::Now()).ToString());
		FString ip = socketClient->resolveDomain(ipOrDomain, ipType);
		int32 portGlobal = port;
		FString clientConnectionIDGlobal = clientConnectionID;
		USocketClientBPLibrary* socketClientGlobal = socketClient;
		USocketClientPluginTCPClient* tcpClientGlobal = tcpClient;
		FString filePath = tcpClient->getFilePath();


		//UE_LOG(LogTemp, Warning, TEXT("Tread:%s:%i"),*ip, port);
		ISocketSubsystem* sSS = USocketClientBPLibrary::getSocketSubSystem();
		if (sSS == nullptr) {
			AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, socketClientGlobal, tcpClientGlobal]() {
				if (socketClientGlobal != nullptr)
					socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(1). SocketSubSystem does not exist." + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				if (tcpClientGlobal != nullptr)
					tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(1). SocketSubSystem does not exist." + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				});
			return 0;
		}
		TSharedRef<FInternetAddr> addr = sSS->CreateInternetAddr();
		bool bIsValid;
		addr->SetIp(*ip, bIsValid);
		addr->SetPort(port);

		if (bIsValid) {
			// create the socket
			FSocket* socket = sSS->CreateSocket(NAME_Stream, TEXT("socketClient"), addr->GetProtocolType());
			tcpClient->setSocket(socket);


			// try to connect to the server
			if (socket == nullptr || socket->Connect(*addr) == false) {
				const TCHAR* socketErr = sSS->GetSocketError(SE_GET_LAST_ERROR_CODE);
				AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, socketClientGlobal, tcpClientGlobal, socketErr]() {
					if (socketClientGlobal != nullptr)
						socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(2):" + FString(socketErr) + "|" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					if (tcpClientGlobal != nullptr)
						tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(2):" + FString(socketErr) + "|" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					});
			}
			else {
				AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal]() {
					if (socketClientGlobal != nullptr)
						socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(true, "Connection successful:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					if (tcpClientGlobal != nullptr)
						tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(true, "Connection successful:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					});


				tcpClient->setRun(true);

				int64 ticks1;
				int64 ticks2;
				TArray<uint8> byteDataArray;
				bool inCollectMessageStatus = false;
				int64 fileSize = 0;
				FString md5Server = FString();

				bool downloadFile = false;
				FArchive* writer = nullptr;
				int64 ticksDownload = FDateTime::Now().GetTicks();
				int64 bytesDownloaded = 0;

				int64 lastByte = 0;

				if (tcpClient->getAesKey().Len() != 32) {
					triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Invalid AES key. ", clientConnectionID, filePath, false, socketClient, tcpClient);
					tcpClient->setRun(false);
				}
				else {
					if (tcpClient->getFileToken().IsEmpty()) {
						triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Invalid token. ", clientConnectionID, filePath, false, socketClient, tcpClient);
						tcpClient->setRun(false);
					}
					else {
						if (tcpClient->isSendFile()) {
							
							if (!FPaths::FileExists(filePath)) {
								triggerFileTransferOverTCPInfoEvent("Data transmission aborted. File not found.", clientConnectionID, filePath, false, socketClient, tcpClient);
								tcpClient->setRun(false);
							}
							else {
								fileSize = tcpClient->fileSize(filePath);
								bool md5okay = false;
								FString md5 = FString();
								tcpClient->getMD5FromFileAbsolutePath(filePath, md5okay, md5);
								FString fileAuthMessage = "SEND_FILE_TO_SERVER_|_" + tcpClient->getFileToken() + "_|_" + md5 + "_|_" + FPaths::GetCleanFilename(filePath) + "_|_" + tcpClient->int64ToString(fileSize) + "\r\n";
								fileAuthMessage = tcpClient->encryptMessage(fileAuthMessage);
								//FString test = tcpClient->decryptMessage(fileAuthMessage);
								//UE_LOG(LogTemp, Display, TEXT("xxxxx %s:"),*test);
								FTCHARToUTF8 Convert(*fileAuthMessage);
								int32 sent = 0;
								socket->Send((uint8*)Convert.Get(), Convert.Length(), sent);
							}
						}
						if (tcpClient->isReceiveFile()) {
							if (!FPaths::DirectoryExists(filePath)) {
								triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Directory not found.", clientConnectionID, filePath, false, socketClient, tcpClient);
								tcpClient->setRun(false);
							}
							else {
								FString fileAuthMessage = "REQUEST_FILE_FROM_SERVER_|_" + tcpClient->getFileToken() + "\r\n";
								fileAuthMessage = tcpClient->encryptMessage(fileAuthMessage);
								//FString test = tcpClient->decryptMessage(fileAuthMessage);
								//UE_LOG(LogTemp, Display, TEXT("xxxxx %s:"),*test);
								FTCHARToUTF8 Convert(*fileAuthMessage);
								int32 sent = 0;
								socket->Send((uint8*)Convert.Get(), Convert.Length(), sent);
							}
						}
					}
				}
				



				uint32 DataSize;
				FArrayReaderPtr Datagram = MakeShareable(new FArrayReader(true));
				while (socket != nullptr && tcpClient->isRun()) {

					//ESocketConnectionState::SCS_Connected does not work https://issues.unrealengine.com/issue/UE-27542
					//Compare ticks is a workaround to get a disconnect. clientSocket->Wait() stop working after disconnect. (Another bug?)
					//If it doesn't wait any longer, ticks1 and ticks2 should be the same == disconnect.
					ticks1 = FDateTime::Now().GetTicks();
					socket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan::FromSeconds(0.1));
					ticks2 = FDateTime::Now().GetTicks();

					bool hasData = socket->HasPendingData(DataSize);
					if (!hasData && ticks1 == ticks2) {
						UE_LOG(LogTemp, Display, TEXT("TCP connection broken. End Loop"));
						break;
					}

					if (hasData) {
						if (downloadFile) {
							//downlnoad
							int32 bytesRead = 0;
							Datagram->Empty();
							Datagram->SetNumUninitialized(DataSize);
							if (socket->Recv(Datagram->GetData(), Datagram->Num(), bytesRead)) {
								writer->Serialize(const_cast<uint8*>(Datagram->GetData()), Datagram->Num());

								//show progress each second
								if ((ticksDownload + 10000000) <= FDateTime::Now().GetTicks()) {
									writer->Flush();
									int64 bytesTransferredLastSecond = bytesDownloaded - lastByte;
									//float speed = ((float)bytesTransferredLastSecond) / 125000;
									float mbit = ((float)bytesTransferredLastSecond) / 1024 / 1024 * 8;
									float sent = ((float)bytesDownloaded) / 1048576;
									float left = 0;
									float percent = 0;
									if (fileSize > 0) {
										left = ((float)(fileSize - bytesDownloaded)) / 1048576;
										percent = ((float)bytesDownloaded / (float)fileSize * 100);
									}


									triggerTransferFileEvent(clientConnectionID, filePath, socketClientGlobal, tcpClient, percent, mbit, bytesDownloaded, fileSize);

									ticksDownload = FDateTime::Now().GetTicks();
									lastByte = bytesDownloaded;
								}
								bytesDownloaded += bytesRead;
							}
							if (bytesDownloaded >= fileSize) {

								writer->Close();
								writer = nullptr;

								sendEndMessage(filePath, tcpClient->getFileToken(), md5Server, clientConnectionID, socket, socketClientGlobal, tcpClient);

								//transfer file finish
								triggerTransferFileEvent(clientConnectionID, filePath, socketClientGlobal, tcpClient, 100, 0, bytesDownloaded, fileSize);
								tcpClient->setRun(false);
							}
						}
						else {
							Datagram->SetNumUninitialized(DataSize);
							int32 BytesRead = 0;
							if (socket->Recv(Datagram->GetData(), Datagram->Num(), BytesRead)) {

								Datagram->Add(0x00);// null-terminator
								char* Data = (char*)Datagram->GetData();
								FString mainMessage = FString(UTF8_TO_TCHAR(Data));
								mainMessage = (tcpClient->decryptMessage(mainMessage)).TrimStartAndEnd();


								if (tcpClient->isReceiveFile()) {
									if (mainMessage.StartsWith("REQUEST_FILE_FROM_SERVER_ACCEPTED_|_" + tcpClient->getFileToken())) {
										TArray<FString> lines;
										mainMessage.ParseIntoArray(lines, TEXT("_|_"), true);


										if (lines.Num() == 5) {
											if (lines[0].Equals("REQUEST_FILE_FROM_SERVER_ACCEPTED") && lines[1].Len() > 0 && lines[2].Len() > 0 && lines[3].Len() > 0 && lines[4].Len() > 0) {
												md5Server= lines[2];
												fileSize = FCString::Atoi64(*lines[3]);
												if (filePath.EndsWith("/")) {
													filePath +=  lines[4];
												}
												else {
													filePath += "/" + lines[4];
												}
												

												

												int64 fileSizeOnClient = 0;
												
												if (FPaths::FileExists(filePath)) {
													fileSizeOnClient = tcpClient->fileSize(filePath);
													if (tcpClient->hasResume()) {
														writer = IFileManager::Get().CreateFileWriter(*filePath, EFileWrite::FILEWRITE_Append);
													}
													else {
														writer = IFileManager::Get().CreateFileWriter(*filePath);
													}
												}
												else {
													writer = IFileManager::Get().CreateFileWriter(*filePath);
												}

												if (writer == nullptr) {
													triggerFileTransferOverTCPInfoEvent("File could not be created. ", clientConnectionID, filePath, false, socketClient, tcpClient);
													tcpClient->setRun(false);
													continue;
												}
											

				
												if (writer->TotalSize() >= fileSize) {
													triggerFileTransferOverTCPInfoEvent("File on the client has the same size or is larger than the file on the server.", clientConnectionID, filePath, false, socketClient, tcpClient);
													tcpClient->setRun(false);
													continue;
												}
																								

												downloadFile = true;
												bytesDownloaded = writer->TotalSize();

												FString fileAuthMessage = "REQUEST_FILE_FROM_SERVER_ACCEPTED_|_" + tcpClient->getFileToken() + "_|_" + tcpClient->int64ToString(bytesDownloaded) + "\r\n";
												fileAuthMessage = tcpClient->encryptMessage(fileAuthMessage);
												FTCHARToUTF8 Convert(*fileAuthMessage);
												int32 sent = 0;
												socket->Send((uint8*)Convert.Get(), Convert.Length(), sent);

												continue;
																			
											}
										}
									}

									triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Wrong answer from the server (3). ", clientConnectionID, filePath, false, socketClient, tcpClient);
									tcpClient->setRun(false);
		
								}
								else {
									if (tcpClient->getFileSendThread() == nullptr && mainMessage.StartsWith("SEND_FILE_TO_SERVER_ACCEPTED_|_" + tcpClient->getFileToken())) {
										if (mainMessage.RemoveFromStart("SEND_FILE_TO_SERVER_ACCEPTED_|_" + tcpClient->getFileToken() + "_|_")) {
											int64 startPosition = FCString::Atoi64(*mainMessage);
											tcpClient->createFileSendThread(startPosition);
										}
										else {
											triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Wrong answer from the server (2). ", clientConnectionID, filePath, false, socketClient, tcpClient);
											tcpClient->setRun(false);
										}
									}
									else {
										if (tcpClient->getFileSendThread() == nullptr) {
											if (mainMessage.RemoveFromStart("SEND_FILE_TO_SERVER_ACCEPTED_|_" + tcpClient->getFileToken() + "_|_")) {
												int64 startPosition = FCString::Atoi64(*mainMessage);
												tcpClient->createFileSendThread(startPosition);
											}
											else {
												bool hasError = true;
												if (mainMessage.StartsWith("SEND_FILE_TO_SERVER_END_|_" + tcpClient->getFileToken())) {
													if (mainMessage.RemoveFromStart("SEND_FILE_TO_SERVER_END_|_" + tcpClient->getFileToken() + "_|_")) {
														if (mainMessage.Equals("OKAY")) {
															triggerFileTransferOverTCPInfoEvent("The file was already complete on the server. ", clientConnectionID, filePath, true, socketClient, tcpClient);
															hasError = false;
														}
													}
												}

												if (hasError) {
													triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Wrong answer from the server (2). ", clientConnectionID, filePath, false, socketClient, tcpClient);
												}
												tcpClient->setRun(false);
												//triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Wrong answer from the server (2). ", clientConnectionID, filePath, false, socketClient, tcpClient);
												//tcpClient->setRun(false);
											}

										}
										else {
											if (mainMessage.StartsWith("SEND_FILE_TO_SERVER_END_|_" + tcpClient->getFileToken())) {
												if (mainMessage.RemoveFromStart("SEND_FILE_TO_SERVER_END_|_" + tcpClient->getFileToken() + "_|_")) {
													if (mainMessage.Equals("OKAY")) {
														triggerFileTransferOverTCPInfoEvent("Data transmission successful. ", clientConnectionID, filePath, true, socketClient, tcpClient);
														tcpClient->setRun(false);
													}
													else {
														triggerFileTransferOverTCPInfoEvent("There was an error during data transmission. File may be corrupted or incomplete. ", clientConnectionID, filePath, false, socketClient, tcpClient);
														tcpClient->setRun(false);
													}
												}
												else {
													triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Wrong answer from the server (3). ", clientConnectionID, filePath, false, socketClient, tcpClient);
													tcpClient->setRun(false);
												}
											}
											else {
												triggerFileTransferOverTCPInfoEvent("Data transmission aborted. Wrong answer from the server (1). ", clientConnectionID, filePath, false, socketClient, tcpClient);
												tcpClient->setRun(false);
											}
										}

									}
								}
							}

						}
						byteDataArray.Empty();
						Datagram->Empty();
					}
				}
				
				if (writer != nullptr) {
					writer->Close();
					writer = nullptr;
				}

				AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal]() {
					if (socketClientGlobal != nullptr) {
						socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection close:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
						if (tcpClientGlobal != nullptr)
							tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection close:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
					}

					});
			}


			//wait for game thread
			//FPlatformProcess::Sleep(1);


			USocketClientBPLibrary::getSocketClientTarget()->closeSocketClientConnectionTCP(clientConnectionID);
		


			tcpClient->setRun(false);
			if (socket != nullptr) {
				socket->Close();
				sSS->DestroySocket(socket);
				socket = nullptr;
			}
			thread = nullptr;
		}
		else {
			AsyncTask(ENamedThreads::GameThread, [ip, portGlobal, clientConnectionIDGlobal, tcpClientGlobal, socketClientGlobal]() {
				if (socketClientGlobal != nullptr)
					socketClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(3). IP not valid:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				if (tcpClientGlobal != nullptr)
					tcpClientGlobal->onsocketClientTCPConnectionEventDelegate.Broadcast(false, "Connection failed(3). IP not valid:" + ip + ":" + FString::FromInt(portGlobal) + "|" + clientConnectionIDGlobal, clientConnectionIDGlobal);
				});
		}

		return 0;
	}

	void triggerFileTransferOverTCPInfoEvent(FString messageP, FString clientConnectionIDP,FString filePathP, bool successP, USocketClientBPLibrary* socketClientP, USocketClientPluginTCPClient* tcpClientP) {
		AsyncTask(ENamedThreads::GameThread, [messageP, clientConnectionIDP, filePathP, successP, socketClientP, tcpClientP]() {
			if (socketClientP != nullptr)
				socketClientP->onfileTransferOverTCPInfoEventDelegate.Broadcast(messageP, clientConnectionIDP, filePathP, successP);
			if (tcpClientP != nullptr)
				tcpClientP->onfileTransferOverTCPInfoEventDelegate.Broadcast(messageP, clientConnectionIDP, filePathP, successP);
			});
	}

	void triggerTransferFileEvent(FString clientConnectionIDP, FString filePathP, USocketClientBPLibrary* socketClientP, USocketClientPluginTCPClient* tcpClientP, float percentP, float mbitP, int64 transferredP, int64 fileSizeP) {
		AsyncTask(ENamedThreads::GameThread, [clientConnectionIDP, socketClientP, filePathP, tcpClientP, percentP, mbitP, transferredP, fileSizeP]() {
			if (socketClientP != nullptr)
				socketClientP->ontransferFileOverTCPProgressEventDelegate.Broadcast(clientConnectionIDP, filePathP, percentP, mbitP, transferredP, fileSizeP);
			if (tcpClientP != nullptr)
				tcpClientP->ontransferFileOverTCPProgressEventDelegate.Broadcast(clientConnectionIDP, filePathP, percentP, mbitP, transferredP, fileSizeP);
			});
	}

	void sendEndMessage(FString fullFilePathP, FString tokenP, FString md5ServerP, FString clientConnectionIDP, FSocket* clientSocketP, USocketClientBPLibrary* socketClientP, USocketClientPluginTCPClient* tcpClientP) {
		bool md5okay = false;
		FString md5Client= FString();
		tcpClientP->getMD5FromFileAbsolutePath(fullFilePathP, md5okay, md5Client);

		FString response = "REQUEST_FILE_FROM_SERVER_END_|_" + tokenP + "_|_";

		if (md5okay && md5ServerP.Equals(md5Client)) {
			triggerFileTransferOverTCPInfoEvent("File successfully received.", clientConnectionIDP, fullFilePathP, true, socketClientP, tcpClientP);

			response += "OKAY\r\n";
		}
		else {
			triggerFileTransferOverTCPInfoEvent("File received but MD5 does not match. Corrupted file will be deleted if resume is not disabled.", clientConnectionIDP, fullFilePathP, false, socketClientP, tcpClientP);
			response += "MD5ERROR\r\n";
			if (tcpClientP->hasResume()) {
				tcpClientP->deleteFile(fullFilePathP);
			}
		}

		response = tcpClientP->encryptMessage(response);

		FTCHARToUTF8 Convert(*response);
		int32 bytesSendOverSocket = 0;
		clientSocketP->Send((uint8*)Convert.Get(), Convert.Length(), bytesSendOverSocket);
	}

protected:
	USocketClientBPLibrary* socketClient = nullptr;
	//USocketClientBPLibrary*		oldClient;
	FString						clientConnectionID;
	FString						originalIP;
	FString ipOrDomain;
	ESocketClientIPType ipType;
	int32 port;
	USocketClientPluginTCPClient* tcpClient = nullptr;



	FRunnableThread* thread = nullptr;
};

/* asynchronous Thread*/
class SOCKETCLIENT_API FSendFileToServerThread : public FRunnable {

public:

	FSendFileToServerThread(USocketClientBPLibrary* socketClientLibP, USocketClientPluginTCPClient* tcpClientP, FString clientConnectionIDP, FString filePathP, int64 startPositionP) :
		socketClientLib(socketClientLibP),
		tcpClient(tcpClientP),
		clientConnectionID(clientConnectionIDP),
		filePath(filePathP),
		startPosition(startPositionP) {
		FString threadName = "FSendFileToServerThread" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {

		if (tcpClient == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("Class is not initialized."));
			return 0;
		}

		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("tcp socket 1"));*/

		FString clientConnectionIDGlobal = clientConnectionID;
		//USocketClientPluginTCPClient* tcpClientGlobal = tcpClient;
		USocketClientBPLibrary* socketClientLibGlobal = socketClientLib;

		// get the socket
		FSocket* socket = tcpClient->getSocket();

		while (tcpClient->isRun()) {

			// try to connect to the server
			if (socket == NULL || socket == nullptr) {
				UE_LOG(LogTemp, Error, TEXT("Connection not exist."));
				//AsyncTask(ENamedThreads::GameThread, [clientConnectionIDGlobal, socketClientLibGlobal]() {
				//	if (socketClientLibGlobal != nullptr) {
				//		socketClientLibGlobal->onsendFileOverTCPEventDelegate.Broadcast("Connection not exist:" + clientConnectionIDGlobal, clientConnectionIDGlobal, 0.f, 0.f, 0l);
				//		socketClientLibGlobal->closeSocketClientConnection();
				//	}
				//});
				break;
			}

			if (socket != nullptr && socket->GetConnectionState() == ESocketConnectionState::SCS_Connected) {
	
				FArchive* reader = IFileManager::Get().CreateFileReader(*filePath);
				if (reader == nullptr || reader->TotalSize() == 0) {
					if (reader != nullptr) {
						reader->Close();
					}
					delete reader;
					break;
				}

	

				int64 fileSize = reader->TotalSize();
				int64 readSize = 0;
				int64 lastPosition = startPosition;
				int64 bytesSentSinceLastTick = 0;

				int32 bufferSize = 1024 * 64;
				int32 dataSendBySocket = 0;
				
				float percent = 0.f;
				float mbit = 0.f;

				TArray<uint8> buffer;

				int64 lastTimeTicks = FDateTime::Now().GetTicks();

				if (bufferSize > fileSize) {
					bufferSize = fileSize;
				}

				if (lastPosition > 0) {
					reader->Seek(lastPosition);
				}

				while (tcpClient->isRun() && lastPosition < fileSize) {
					if ((lastPosition + bufferSize) > fileSize) {
						bufferSize = fileSize - lastPosition;
					}

					//buffer.Reset(bufferSize);
					buffer.Empty();
					buffer.AddUninitialized(bufferSize);
					
					reader->Serialize(buffer.GetData(), buffer.Num());
					lastPosition += buffer.Num();

					//socket->Send((uint8*)((ANSICHAR*)Convert.Get()), Convert.Length(), dataSendBySocket);
					socket->Send(buffer.GetData(), buffer.Num(), dataSendBySocket);

					//slowdown for tests
					//FPlatformProcess::Sleep(0.01f);

					//fire event every second
					//one second = 10000000 ticks
					if (((FDateTime::Now().GetTicks()) - lastTimeTicks) >= 10000000) {
						mbit = ((float)lastPosition - (float)bytesSentSinceLastTick) / 1024 / 1024 * 8;
						lastTimeTicks = FDateTime::Now().GetTicks();
						bytesSentSinceLastTick = lastPosition;

						percent = ((float)lastPosition / fileSize) * 100;
						triggerTransferFileEvent(clientConnectionID, filePath, socketClientLib, tcpClient, percent, mbit, lastPosition, fileSize);
					}

				}

				mbit = ((float)lastPosition - (float)bytesSentSinceLastTick) / 1024 / 1024 * 8;
				percent = ((float)lastPosition / fileSize) * 100;

				triggerTransferFileEvent(clientConnectionID, filePath, socketClientLib, tcpClient, percent, mbit, lastPosition, fileSize);

				buffer.Empty();
				if (reader != nullptr) {
					reader->Close();
					reader = nullptr;
				}
				break;


			}

		}
		thread = nullptr;
		return 0;
	}

	void triggerTransferFileEvent(FString clientConnectionIDP, FString filePathP, USocketClientBPLibrary* socketClientP, USocketClientPluginTCPClient* tcpClientP, float percentP, float mbitP, int64 transferredP, int64 fileSizeP) {
		AsyncTask(ENamedThreads::GameThread, [clientConnectionIDP, socketClientP, filePathP, tcpClientP, percentP, mbitP, transferredP, fileSizeP]() {
			if (socketClientP != nullptr)
				socketClientP->ontransferFileOverTCPProgressEventDelegate.Broadcast(clientConnectionIDP, filePathP, percentP, mbitP, transferredP, fileSizeP);
			if (tcpClientP != nullptr)
				tcpClientP->ontransferFileOverTCPProgressEventDelegate.Broadcast(clientConnectionIDP, filePathP, percentP, mbitP, transferredP, fileSizeP);
			});
	}


protected:
	USocketClientBPLibrary* socketClientLib;
	USocketClientPluginTCPClient* tcpClient = nullptr;
	FString clientConnectionID;
	FString filePath;
	int64 startPosition = 0;
	FRunnableThread* thread = nullptr;
	bool					run;
};
