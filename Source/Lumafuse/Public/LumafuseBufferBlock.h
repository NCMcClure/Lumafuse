#pragma once

#include "LumafuseBufferBlockPayload.h"
#include "LumafuseBufferBlock.generated.h"

USTRUCT(BlueprintType)
struct FLumafuseBufferBlock
{
	GENERATED_BODY()

	TMap<int32, FLumafuseBufferBlockPayload> BufferBlockPayloads;

	bool ConstructionCompleted;
	
};
