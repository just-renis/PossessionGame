#pragma once

#include "FNiagaraAttribute.generated.h"

USTRUCT()
struct POSSESSIONGAME_API FNiagaraAttribute
{
	GENERATED_BODY()

	FNiagaraAttribute() : bIsReplacingValue(false), Value(0.f) {}
	FNiagaraAttribute(const bool bInReplace, const float InFloat) : bIsReplacingValue(bInReplace), Value(InFloat) {}

	bool bIsReplacingValue;
	float Value;
};
