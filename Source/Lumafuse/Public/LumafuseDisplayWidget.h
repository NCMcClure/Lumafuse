// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LumaFuseBufferBlock.h"
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
	
	
	struct FBufferBlock
	{
		TArray<uint8> Buffer;
		bool ConstructionComplete;
	};

	UFUNCTION(BlueprintCallable, Category = "Lumafuse | Display Rendering")
	static void SortPacketIntoFrameBlockBuffer(TArray<uint8>& Packet, TMap<int32, FLumaFuseBufferBlock>& SortedBufferBlock, int32 NumberOfBlockPackets, bool& BlockCompleted);
	
	void TryRenderFrame(TArray<uint8>& Packet);

};
