#pragma once

#include "FNiagaraVariationData.generated.h"

USTRUCT()
struct FNiagaraVariationData
{
	GENERATED_BODY()

	FNiagaraVariationData() : bRequiredVfxChange(false), Hue(0.f) {}
	FNiagaraVariationData(const bool bIsPlayerVfx, const float InValue) : bRequiredVfxChange(bIsPlayerVfx), Hue(InValue) {}

	UPROPERTY(EditDefaultsOnly)
	bool bRequiredVfxChange;
	UPROPERTY(EditDefaultsOnly)
	float Hue;
};
