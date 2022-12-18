#pragma once

#include "LumafuseBufferBlock.h"
#include "LumafuseFrameBuffer.generated.h"

USTRUCT(BlueprintType)
struct FLumafuseFrameBuffer
{
	GENERATED_BODY()

	TMap<FIntPoint, FLumafuseBufferBlock> FrameBufferBlocks;

	bool Rendered = false;
	
};
