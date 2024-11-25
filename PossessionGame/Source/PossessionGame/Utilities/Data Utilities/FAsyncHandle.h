#pragma once

#include "FAsyncHandle.generated.h"

struct FStreamableHandle;

USTRUCT()
struct FAsyncHandle
{
	GENERATED_BODY()
	
	TSharedPtr<FStreamableHandle> Handle;
};
