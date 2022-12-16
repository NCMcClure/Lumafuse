#pragma once

#include "LumaFuseBufferBlock.generated.h"

USTRUCT(BlueprintType)
struct FLumaFuseBufferBlock
{
	TArray<uint8> Buffer;

	bool ConstructionCompleted;
	
};
