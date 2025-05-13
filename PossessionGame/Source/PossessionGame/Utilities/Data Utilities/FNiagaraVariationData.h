#pragma once

#include "FNiagaraVariationData.generated.h"

USTRUCT()
struct FNiagaraVariationData
{
	GENERATED_BODY()

	FNiagaraVariationData() : bRequiredVfxChange(false), Hue(0.f), Color() {}
	FNiagaraVariationData(const bool bIsPlayerVfx, const float InValue, const FLinearColor InColor = FLinearColor()) : bRequiredVfxChange(bIsPlayerVfx), Hue(InValue), Color(InColor) {}

	UPROPERTY(EditDefaultsOnly)
	bool bRequiredVfxChange;
	UPROPERTY(EditDefaultsOnly)
	float Hue;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor Color;
};
