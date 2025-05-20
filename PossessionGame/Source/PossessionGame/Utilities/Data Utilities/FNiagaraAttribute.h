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
	FNiagaraAttribute(const bool bInReplace, const TObjectPtr<UMaterialInterface>& InMaterial) : bIsReplacingValue(bInReplace), Value(TInPlaceType<TObjectPtr<UMaterialInterface>>(), InMaterial) {}

	bool bIsReplacingValue;
	TVariant<float, FVector, FLinearColor, TObjectPtr<UMaterialInterface>> Value;
	
	bool IsFloat() const { return Value.IsType<float>(); }
	bool IsVector() const { return Value.IsType<FVector>(); }
	bool IsColor() const { return Value.IsType<FLinearColor>(); }
	bool IsMaterial() const { return Value.IsType<TObjectPtr<UMaterialInterface>>(); }
	float GetFloat() const { return Value.Get<float>(); }
	FVector GetVector() const { return Value.Get<FVector>(); }
	FLinearColor GetColor() const { return Value.Get<FLinearColor>(); }
	TObjectPtr<UMaterialInterface> GetMaterial() const { return Value.Get<TObjectPtr<UMaterialInterface>>(); }
};
