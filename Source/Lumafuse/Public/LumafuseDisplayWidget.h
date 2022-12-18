// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LumafuseBufferBlock.h"
#include "LumafuseBufferBlockPayload.h"
#include "LumafuseFrameBuffer.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "LowEntryExtendedStandardLibrary/Public/Classes/LowEntryExtendedStandardLibrary.h"
#include "LumafuseDisplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUMAFUSE_API ULumafuseDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	
	static void FoldPacketIntoFrameBufferBlock(TArray<uint8>& Packet, TMap<int32, FLumafuseBufferBlockPayload>& SortedBufferBlock, int32 NumberOfBlockPackets, bool& BlockCompleted);

	UFUNCTION(BlueprintCallable, Category = "Lumafuse | Display Rendering")
	void TryRenderFrame(UPARAM(ref)TArray<uint8>& Packet, UPARAM(ref)TMap<uint8, FLumafuseFrameBuffer>& FrameQueue, UPARAM(ref)TMap<FIntPoint, UTexture2D*>& TextureBlocks);

	void ConvertBufferBlockToTexture(TMap<int32, FLumafuseBufferBlockPayload>& SortedBufferBlock, UTexture2D* TextureBlock);

};
