#pragma once

#include "FTFunctionCallback.generated.h"

USTRUCT()
struct FTFunctionCallback
{
	GENERATED_BODY()
	
	TFunction<void()> Function;
};
