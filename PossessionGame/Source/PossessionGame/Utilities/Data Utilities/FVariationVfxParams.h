#pragma once

#include "PossessionGame/Enums/EUserParamType.h"
#include "FVariationVfxParams.generated.h"

USTRUCT()
struct FDynamicMaterialParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EUserParamType ParamType = EUserParamType::Float;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Float", EditConditionHides))
	float FloatValue = 0.f;
};

USTRUCT()
struct FNiagaraUserParam
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EUserParamType ParamType = EUserParamType::Float;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Bool", EditConditionHides))
	bool bBoolValue = false;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Int", EditConditionHides))
	int32 IntValue = 0;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Float", EditConditionHides))
	float FloatValue = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Vector", EditConditionHides))
	FVector VectorValue = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Color", EditConditionHides))
	FLinearColor ColorValue = FLinearColor::White;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Object", EditConditionHides))
	TObjectPtr<UObject> Object = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Material", EditConditionHides))
	TObjectPtr<UMaterialInterface> MaterialValue = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Material", EditConditionHides))
	TMap<FName, FDynamicMaterialParams> MaterialParams;
};

USTRUCT()
struct FVariationVfxParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Vfx Data", meta = (AllowPrivateAccess = true))
	TMap<FName, FNiagaraUserParam> Params;
};
