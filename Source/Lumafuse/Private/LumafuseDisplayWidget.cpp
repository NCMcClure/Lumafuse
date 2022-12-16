// Fill out your copyright notice in the Description page of Project Settings.

#include "LumafuseDisplayWidget.h"

void ULumafuseDisplayWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Create a TSortedMap with the keys being a uint8 and the values being another TSortedMap of FIntPoint and UTexture2D
	// This is so we can sort the keys and then sort the values within each key
	TSortedMap<uint8, TSortedMap<FIntPoint, TArray<uint8>>> RawFrameQueue;
	

}

void ULumafuseDisplayWidget::SortPacketIntoFrameBlockBuffer(TArray<uint8>& Packet,
	TMap<int32, FLumaFuseBufferBlock>& SortedBufferBlock, int32 NumberOfBlockPackets, bool& BlockCompleted)
{
	//Using a TSortedMap, we can sort the buffer block from the packet into the frame buffer
	//based on the block coordinate. This will ensure that the frame buffer is in the correct order
	//when it is finally constructed.

	int32 PacketHeaderLength = 27;
	
	int32 PayloadBlockIndex = ULowEntryExtendedStandardLibrary::BytesToInteger(
		ULowEntryExtendedStandardLibrary::BytesSubArray(Packet, 18, 4));

	//Extracting the payload from the packet by removing the header
	TArray<uint8> Payload = Packet;
	Payload.RemoveAt(0, PacketHeaderLength);

	//Checking to see if the block is completed
	if (SortedBufferBlock.Num() == NumberOfBlockPackets)
	{
		BlockCompleted = true;
	}

	FLumaFuseBufferBlock Block;
	Block.Buffer = Payload;
	Block.ConstructionCompleted = BlockCompleted;

	//Adding the payload to the sorted buffer block
	SortedBufferBlock.Add(PayloadBlockIndex, Block);

	//Sort the buffer block if the block is greater than 1
	if (SortedBufferBlock.Num() > 1)
	{
		//Sort SortedBufferBlock by the key in ascending order
		SortedBufferBlock.KeySort([](const int32& A, const int32& B) { return A < B; });
	}
}

void ULumafuseDisplayWidget::TryRenderFrame(TArray<uint8>& Packet)
{
	
}
