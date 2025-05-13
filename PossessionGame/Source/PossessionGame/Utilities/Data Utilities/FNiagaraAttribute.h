#pragma once

#include "FNiagaraAttribute.generated.h"

USTRUCT()
struct FNiagaraAttribute
{
	GENERATED_BODY()

	FNiagaraAttribute() : bIsReplacingValue(false), Value(TInPlaceType<float>(), 0.f) {}
	FNiagaraAttribute(const bool bInReplace, const float InFloat) : bIsReplacingValue(bInReplace), Value(TInPlaceType<float>(), InFloat) {}
	FNiagaraAttribute(const bool bInReplace, const FVector& InVector) : bIsReplacingValue(bInReplace), Value(TInPlaceType<FVector>(), InVector) {}
	FNiagaraAttribute(const bool bInReplace, const FLinearColor& InColor) : bIsReplacingValue(bInReplace), Value(TInPlaceType<FLinearColor>(), InColor) {}

	bool bIsReplacingValue;
	TVariant<float, FVector, FLinearColor> Value;
	
	bool IsFloat() const { return Value.IsType<float>(); }
	bool IsVector() const { return Value.IsType<FVector>(); }
	bool IsColor() const { return Value.IsType<FLinearColor>(); }
	float GetFloat() const { return Value.Get<float>(); }
	FVector GetVector() const { return Value.Get<FVector>(); }
	FLinearColor GetColor() const { return Value.Get<FLinearColor>(); }
};
