#pragma once

#include "LumafuseBufferBlockPayload.generated.h"

USTRUCT(BlueprintType)
struct FLumafuseBufferBlockPayload
{
	GENERATED_BODY()

	TArray<uint8> Payload;
};
