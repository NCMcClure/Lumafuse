// Copyright 2017-2019 David Romanski (Socke). All Rights Reserved.
#pragma once

#include "SocketClient.h"
#include "SocketClientBPLibrary.h"
#include "FileFunctionsSocketClient.generated.h"

class FReadFileInPartsSocketClientThread;


UCLASS(Blueprintable, BlueprintType)
class SOCKETCLIENT_API UFileFunctionsSocketClient : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION()
		static UFileFunctionsSocketClient* getFileFunctionsSocketClientTarget();
	static UFileFunctionsSocketClient* fileFunctionsSocketClient;




	static FString getCleanDirectory(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);

	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void writeBytesToFile(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, TArray<uint8> bytes, bool& success);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void addBytesToFileAndCloseIt(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, TArray<uint8> bytes, bool& success);
	//UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
	//	static void splittFile(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, int32 parts, bool& success);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static TArray<uint8> readBytesFromFile(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, bool &success);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void readStringFromFile(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, bool& success, FString& data);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void writeStringToFile(EFileFunctionsSocketClientDirectoryType directoryType, FString data, FString filePath, EFileFunctionsSocketClientEncodingOptions fileEncoding, bool& success);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void getMD5FromFile(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, bool& success, FString& MD5);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void getMD5FromFileAbsolutePath(FString filePath, bool& success, FString& MD5);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void stringToBase64String(FString string, FString& base64String);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void base64StringToString(FString& string, FString base64String);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void bytesToBase64String(TArray<uint8> bytes, FString& base64String);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static TArray<uint8> base64StringToBytes(FString base64String, bool& success);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void fileToBase64String(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, bool& success, FString& base64String, FString& fileName);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool fileExists(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool fileExistsAbsolutePath(FString filePath);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool directoryExists(EFileFunctionsSocketClientDirectoryType directoryType, FString path);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static int64 fileSize(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static int64 fileSizeAbsolutePath(FString filePath);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool deleteFile(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	UFUNCTION(BlueprintCallable, Category = "SocketServer|SpecialFunctions|File")
		static bool deleteFileAbsolutePath(FString filePath);
	/** Delete a directory and return true if the directory was deleted or otherwise does not exist. **/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool deleteDirectory(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	/** Return true if the file is read only. **/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool isReadOnly(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	/** Attempt to move a file. Return true if successful. Will not overwrite existing files. **/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool moveFile(EFileFunctionsSocketClientDirectoryType directoryTypeTo, FString filePathTo, EFileFunctionsSocketClientDirectoryType directoryTypeFrom, FString filePathFrom);
	/** Attempt to change the read only status of a file. Return true if successful. **/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool setReadOnly(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, bool bNewReadOnlyValue);
	/** Return the modification time of a file. Returns FDateTime::MinValue() on failure **/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static FDateTime getTimeStamp(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	/** Sets the modification time of a file **/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void	setTimeStamp(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, FDateTime DateTime);
	/** Return the last access time of a file. Returns FDateTime::MinValue() on failure **/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static FDateTime getAccessTimeStamp(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	/** For case insensitive filesystems, returns the full path of the file with the same case as in the filesystem */
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static FString getFilenameOnDisk(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	/** Create a directory and return true if the directory was created or already existed. **/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static bool createDirectory(EFileFunctionsSocketClientDirectoryType directoryType, FString path);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void getAllFilesFromDirectory(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, int32& count, TArray<FString>& files, TArray<FString>& filePaths, FString fileType ="*.*");

	/**
	* Encrypts a string with AES in 256bit and returns the encrypted string as Base64 string.
	* @param message The string to be encrypted
	* @param keyIn256Bit The key must be a string with 32 characters. Please use ASCII characters only!
	*/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|AES")
		static FString encryptMessageWithAES(FString message, FString keyIn256Bit);

	/**
	* Decrypts a Base64 string that has been encrypted in AES with 256bit and returns the string.
	* @param message The string to be decrypted
	* @param keyIn256Bit The key must be a string with 32 characters. Please use ASCII characters only!
	*/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|AES")
		static FString decryptMessageWithAES(FString encryptedBase64Message, FString keyIn256Bit);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SocketClient|SpecialFunctions|String")
		static FString int64ToString(int64 num);



	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static struct FFileFunctionsSocketClientOpenFile openFile(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static int64 addBytesToFile(struct FFileFunctionsSocketClientOpenFile openFile, TArray<uint8>bytes);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void closeFile(struct FFileFunctionsSocketClientOpenFile openFile);

	/**
	* With this function you can read a file piece by piece. This reduces the RAM consumption to almost zero and files can be read in infinite size.
	*@param bufferSize In bytes. This is the size of the file pieces that are being read.
	*@param delayBetweenReadsInSeconds Specified in seconds. The higher the value, the slower the file is read (0.0001 minimum). When sending data over the network/internet, please make sure not to send data too fast. SSDs can read data much faster than you can send it over a network. This means that the data ends up in some buffers (RAM) and can also cause them to overflow.
	*/
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File", meta = (AdvancedDisplay = 2))
		static void readBytesFromFileInPartsAsync(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, int32 bufferSize = 65536, float delayBetweenReadsInSeconds = 0.01f);
	void readBytesFromFileInPartsAsyncInternal(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, int32 bufferSize = 65536, float delayBetweenReadsInSeconds = 0.01f);
	UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
		static void cancelReadBytesFromFileInParts(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	void cancelReadBytesFromFileInPartsInternal(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath);
	void cleanReadBytesFromFileInParts(FString cleanDir);
	//UFUNCTION(BlueprintCallable, Category = "SocketClient|SpecialFunctions|File")
	//	static void changeDelayInBytesFromFileInParts(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, float delayBetweenReadsInSeconds = 0.1f);
	//void changeDelayInBytesFromFileInPartsInternal(EFileFunctionsSocketClientDirectoryType directoryType, FString filePath, float delayBetweenReadsInSeconds = 0.1f);


	TMap<FString, FReadFileInPartsSocketClientThread*> readFileInPartsThreads;

private:
		static TArray<uint8> FStringToByteArray(FString s);
		

};

/* asynchronous Thread*/
class SOCKETCLIENT_API FReadFileInPartsSocketClientThread : public FRunnable {

public:

	FReadFileInPartsSocketClientThread(FString cleanDirP, int32 bufferSizeP, float delayBetweenReadsInSecondsP) :
		cleanDir(cleanDirP),
		bufferSize(bufferSizeP),
		delayBetweenReadsInSeconds(delayBetweenReadsInSecondsP)
	{
		FString threadName = "FReadFileInPartsSocketClientThread" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {

		FArchive* reader = IFileManager::Get().CreateFileReader(*cleanDir);
		if (reader == nullptr || reader->TotalSize() == 0) {
			AsyncTask(ENamedThreads::GameThread, []() {
				USocketClientBPLibrary::getSocketClientTarget()->onreadBytesFromFileInPartsEventDelegate.Broadcast(0, 0, true, TArray<uint8>());
				});
			if (reader != nullptr) {
				reader->Close();
			}
			delete reader;
			return 0;
		}

		if (delayBetweenReadsInSeconds <= 0) {
			delayBetweenReadsInSeconds = 0.0001f;
		}

		int64 fileSize = reader->TotalSize();
		int64 readSize = 0;
		int64 lastPosition = 0;
		TArray<uint8> buffer;

		if (bufferSize > fileSize) {
			bufferSize = fileSize;
		}

		while (run && lastPosition < fileSize) {
			if ((lastPosition + bufferSize) > fileSize) {
				bufferSize = fileSize - lastPosition;
			}

			//buffer.Reset(bufferSize);
			buffer.Empty();
			buffer.AddUninitialized(bufferSize);

			reader->Serialize(buffer.GetData(), buffer.Num());
			lastPosition += buffer.Num();

			//UE_LOG(LogTemp, Warning, TEXT("xxxxx READ: %i"), buffer.Num());


			AsyncTask(ENamedThreads::GameThread, [fileSize, lastPosition, buffer]() {
				USocketClientBPLibrary::getSocketClientTarget()->onreadBytesFromFileInPartsEventDelegate.Broadcast(fileSize, lastPosition, false, buffer);
			});

			FPlatformProcess::Sleep(delayBetweenReadsInSeconds);

		}

		AsyncTask(ENamedThreads::GameThread, [fileSize, lastPosition]() {
			USocketClientBPLibrary::getSocketClientTarget()->onreadBytesFromFileInPartsEventDelegate.Broadcast(fileSize, lastPosition, true, TArray<uint8>());
			});

		UFileFunctionsSocketClient::getFileFunctionsSocketClientTarget()->cleanReadBytesFromFileInParts(cleanDir);
		//buffer.Empty();
		if (reader != nullptr) {
			reader->Close();
		}
		delete reader;
		thread = nullptr;
		return 0;
	}

	void stopThread() {
		run = false;
	}

	void setDelayBetweenReadsInSeconds(float d) {
		delayBetweenReadsInSeconds = d;
		if (delayBetweenReadsInSeconds <= 0) {
			delayBetweenReadsInSeconds = 0.001f;
		}
	}


protected:
	bool run = true;
	FString cleanDir;
	int32 bufferSize;
	float delayBetweenReadsInSeconds;
	//USocketClientBPLibrary* mainLib = USocketClientBPLibrary::getSocketClientTarget();

	FRunnableThread* thread = nullptr;
};
