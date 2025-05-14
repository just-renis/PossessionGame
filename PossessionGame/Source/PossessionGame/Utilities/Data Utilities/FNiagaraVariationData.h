#pragma once

#include "FNiagaraVariationData.generated.h"

USTRUCT()
struct FNiagaraVariationData
{
	GENERATED_BODY()

	FNiagaraVariationData() : bRequiredVfxChange(false), Hue(0.f), Color() {}
	explicit FNiagaraVariationData(const bool bIsRequiredVfxChange, const float InValue, const FLinearColor InColor = FLinearColor())
	: bRequiredVfxChange(bIsRequiredVfxChange), Hue(InValue), Color(InColor) {}

	UPROPERTY(EditDefaultsOnly)
	bool bRequiredVfxChange;
	UPROPERTY(EditDefaultsOnly)
	float Hue;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor Color;
};
