// Fill out your copyright notice in the Description page of Project Settings.

#include "LumafuseDisplayWidget.h"

void ULumafuseDisplayWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void ULumafuseDisplayWidget::FoldPacketIntoFrameBufferBlock(TArray<uint8>& Packet,
	TMap<int32, FLumafuseBufferBlockPayload>& SortedBufferBlock, int32 NumberOfBlockPackets, bool& BlockCompleted)
{
	//Using a TMap, we can sort the buffer block from the packet into the frame buffer
	//based on the block coordinate. This will ensure that the frame buffer is in the correct order
	//when it is finally constructed.

	UE_LOG(LogTemp, Warning, TEXT("FOLDING - Setting header parameters"));

	int32 PacketHeaderLength = 27;
	
	int32 PayloadBlockIndex = ULowEntryExtendedStandardLibrary::BytesToInteger(
		ULowEntryExtendedStandardLibrary::BytesSubArray(Packet, 18, 4));

	UE_LOG(LogTemp, Warning, TEXT("FOLDING - Extracting payload from packet"));
	//Extracting the payload from the packet by removing the header
	TArray<uint8> Payload = Packet;
	Payload.RemoveAt(0, PacketHeaderLength);

	UE_LOG(LogTemp, Warning, TEXT("FOLDING - Checking if block is complete"));
	//Checking to see if the block is completed
	if (SortedBufferBlock.Num() == NumberOfBlockPackets)
	{
		UE_LOG(LogTemp, Warning, TEXT("FOLDING - Block is complete"));
		BlockCompleted = true;
	}

	UE_LOG(LogTemp, Warning, TEXT("FOLDING - Initializing buffer block payload"));
	FLumafuseBufferBlockPayload BlockPayload;
	BlockPayload.Payload = Payload;

	UE_LOG(LogTemp, Warning, TEXT("FOLDING - Adding payload to block"));
	//Adding the payload to the sorted buffer block
	SortedBufferBlock.Add(PayloadBlockIndex, BlockPayload);
	
	//Sort the buffer block when the block is greater than 1
	if (SortedBufferBlock.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("FOLDING - Sorting the buffer block"));
		//Sort SortedBufferBlock by the key in ascending order
		SortedBufferBlock.KeySort([](const int32& A, const int32& B) { return A < B; });
	}
}

void ULumafuseDisplayWidget::TryRenderFrame(TArray<uint8>& Packet, TMap<uint8, FLumafuseFrameBuffer>& FrameQueue, TMap<FIntPoint, UTexture2D*>& TextureBlocks)
{

	// Find any FrameQueue entries with Rendered set to true and remove them from the FrameQueue
	// This is to ensure that the FrameQueue does not grow too large

	UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Checking for Rendered frames"));
	TArray<uint8> FrameQueueKeys;
	FrameQueue.GetKeys(FrameQueueKeys);
	for (uint8 FrameQueueKey : FrameQueueKeys)
	{
		if (FrameQueue[FrameQueueKey].Rendered)
		{
			UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Removing already rendered frame: %d"), FrameQueueKey);
			FrameQueue.Remove(FrameQueueKey);
		}
	}

	// Getting some of the key header values from the packet
	UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Getting key header values"));
	uint8 FrameID = ULowEntryExtendedStandardLibrary::BytesToInteger(
		ULowEntryExtendedStandardLibrary::BytesSubArray(Packet, 1, 1));
	
	int32 PacketsInBlock = ULowEntryExtendedStandardLibrary::BytesToInteger(
		ULowEntryExtendedStandardLibrary::BytesSubArray(Packet, 22, 4));
	
	FIntPoint PacketBlockCoordinate = FIntPoint(
		ULowEntryExtendedStandardLibrary::BytesToInteger(
			ULowEntryExtendedStandardLibrary::BytesSubArray(Packet, 10, 4)),
		ULowEntryExtendedStandardLibrary::BytesToInteger(
			ULowEntryExtendedStandardLibrary::BytesSubArray(Packet, 14, 4)));

	int32 NumberOfHorizontalBlocks = ULowEntryExtendedStandardLibrary::BytesToInteger(
		ULowEntryExtendedStandardLibrary::BytesSubArray(Packet, 2, 4));

	int32 NumberOfVerticalBlocks = ULowEntryExtendedStandardLibrary::BytesToInteger(
	ULowEntryExtendedStandardLibrary::BytesSubArray(Packet, 6, 4));

	
	// Debug log to check the values
	UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - FrameID: %d"), FrameID);
	UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - PacketsInBlock: %d"), PacketsInBlock);
	UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - PacketBlockCoordinate: %s"), *PacketBlockCoordinate.ToString());

	// If FrameQueue[FrameID] is not initialized, initialize it so that there is a valid TMap for FoldPacketIntoFrameBufferBlock
	if (!FrameQueue.Contains(FrameID))
	{

		UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Initializing FrameQueue pair since one doesn't exist for current FrameID"));
		// Initializing an empty FrameBuffer TMap and adding it to the FrameQueue
		FLumafuseBufferBlockPayload BufferBlockPayload;
		TArray<uint8> BlankPayload;
		BufferBlockPayload.Payload.Append(BlankPayload);

		FLumafuseBufferBlock BufferBlock;
		BufferBlock.BufferBlockPayloads.Add(0, BufferBlockPayload);
		BufferBlock.ConstructionCompleted = false;
		
		FLumafuseFrameBuffer FrameBuffer;
		for (int32 HorizontalBlock = 0; HorizontalBlock < NumberOfHorizontalBlocks; HorizontalBlock++)
		{
			for (int32 VerticalBlock = 0; VerticalBlock < NumberOfVerticalBlocks; VerticalBlock++)
			{
				FrameBuffer.FrameBufferBlocks.Add(FIntPoint(HorizontalBlock, VerticalBlock), BufferBlock);
				//Debug log to check the values
				UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Adding empty BufferBlock to FrameBuffer: %s"), *FIntPoint(HorizontalBlock, VerticalBlock).ToString());
			}
		}
		FrameBuffer.Rendered = false;

		FrameQueue.Add(FrameID, FrameBuffer);
	}

	// Add the buffer payload to the frame queue of blocks
	UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Folding packet into frame buffer block"));

	FoldPacketIntoFrameBufferBlock(Packet, FrameQueue[FrameID].FrameBufferBlocks[PacketBlockCoordinate].BufferBlockPayloads,
	                               PacketsInBlock,
	                               FrameQueue[FrameID].FrameBufferBlocks[PacketBlockCoordinate].ConstructionCompleted);

	
	FLumafuseFrameBuffer TempFrameBuffer;
	TempFrameBuffer.FrameBufferBlocks = FrameQueue[FrameID].FrameBufferBlocks;
		
	FrameQueue.Add(FrameID, TempFrameBuffer);

	// Loop through the frame buffer and check if all the blocks are completed
	// If they are, then we can render the frame
	bool AllBlocksInCurrentFrameConstructed = false;

	UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Checking if all number of frame buffer blocks is equal to the number of needed texture blocks"));

	if (FrameQueue[FrameID].FrameBufferBlocks.Num() == TextureBlocks.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Checking if all blocks in current frame are constructed"));
		for (auto& Block : FrameQueue[FrameID].FrameBufferBlocks)
		{
			if (!Block.Value.ConstructionCompleted)
			{
				UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - NOT all blocks in current frame are constructed, breaking loop"));
				AllBlocksInCurrentFrameConstructed = false;
				break;
			}
			if (Block.Value.ConstructionCompleted)
			{
				AllBlocksInCurrentFrameConstructed = true;
			}
		}
		// If all of the blocks in the current frame have been constructed, then update the frame block textures
		if (AllBlocksInCurrentFrameConstructed)
		{
			UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - All blocks in current frame are constructed, rendering frame"));
			//Render the frame by setting the brush of the all of the image blocks to the texture
			for (auto& Block : FrameQueue[FrameID].FrameBufferBlocks)
			{
				UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - All blocks in current frame are constructed! CONVERTING BUFFER BLOCK TO TEXTURE"));
				ConvertBufferBlockToTexture(Block.Value.BufferBlockPayloads, TextureBlocks[Block.Key]);

				UE_LOG(LogTemp, Warning, TEXT("TRY RENDER - Rendering completed. Setting Rendered to true for current block"));
				FrameQueue[FrameID].Rendered = true;
			}
		}
	}
}

void ULumafuseDisplayWidget::ConvertBufferBlockToTexture(TMap<int32, FLumafuseBufferBlockPayload>& SortedBufferBlock, UTexture2D* TextureBlock)
{
	//Create a new array to hold the payload
	TArray<uint8> Buffer;
	
	//Iterate through the sorted buffer block and add the payload to the Payload array
	for (auto& BlockPayload : SortedBufferBlock)
	{
		Buffer.Append(BlockPayload.Value.Payload);
	}

	bool ReusedTexture;

	ULowEntryExtendedStandardLibrary::BytesToExistingImage(ReusedTexture, TextureBlock, Buffer, ELowEntryImageFormat::JPEG);
}