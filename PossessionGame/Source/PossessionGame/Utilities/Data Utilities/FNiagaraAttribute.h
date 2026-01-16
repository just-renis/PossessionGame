#pragma once

#include "PossessionGame/Enums/EUserParamType.h"
#include "FNiagaraAttribute.generated.h"

USTRUCT()
struct POSSESSIONGAME_API FNiagaraAttribute
{
	GENERATED_BODY()

	FNiagaraAttribute() : ParamType(EUserParamType::Float), FloatValue(0.f), VectorValue(FVector::ZeroVector) {}
	FNiagaraAttribute(const EUserParamType InParamType, const float InFloat) : ParamType(InParamType), FloatValue(InFloat), VectorValue() {}
	FNiagaraAttribute(const EUserParamType InParamType, const FVector& InVector) : ParamType(InParamType), FloatValue(), VectorValue(InVector) {}

	EUserParamType ParamType;
	float FloatValue;
	FVector VectorValue;
};
