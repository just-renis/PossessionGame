#pragma once

#include "FNiagaraAttribute.generated.h"

USTRUCT()
struct FNiagaraAttribute
{
	GENERATED_BODY()

	FNiagaraAttribute() : bIsReplacingValue(false), Value(0.f) {}
	FNiagaraAttribute(const bool bIsReplace, const float InValue) : bIsReplacingValue(bIsReplace), Value(InValue) {}
	
	bool bIsReplacingValue;
	float Value;
};
