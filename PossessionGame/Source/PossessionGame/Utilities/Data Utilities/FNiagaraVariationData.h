#pragma once

#include "FNiagaraVariationData.generated.h"

USTRUCT()
struct FNiagaraVariationData
{
	GENERATED_BODY()

	FNiagaraVariationData() : bRequiredVfxChange(false), Hue(0.f), Color() {}
	explicit FNiagaraVariationData(const float InValue, const FLinearColor InColor = FLinearColor()) : Hue(InValue), Color(InColor) {}

	UPROPERTY()
	bool bRequiredVfxChange = true;
	UPROPERTY(EditDefaultsOnly)
	float Hue;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor Color;
};
