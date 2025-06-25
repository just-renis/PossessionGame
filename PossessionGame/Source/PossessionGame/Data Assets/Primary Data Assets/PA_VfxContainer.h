#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_VfxContainer.generated.h"

enum class EVariation : uint8;
class UNiagaraSystem;

UENUM()
enum class EUserParamType : uint8
{
	Int,
	Float,
	Vector,
	Color,
	Material
};

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
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Int", EditConditionHides))
	int32 IntValue = 0;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Float", EditConditionHides))
	float FloatValue = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Vector", EditConditionHides))
	FVector VectorValue = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ParamType == EUserParamType::Color", EditConditionHides))
	FLinearColor ColorValue = FLinearColor::White;
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

UCLASS()
class POSSESSIONGAME_API UPA_VfxContainer : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	FName GetVfxName() const { return VfxPrimaryName; }
	TSoftObjectPtr<UNiagaraSystem>& GetVfxEffect() { return VfxEffect; }
	TMap<FName, FNiagaraUserParam>* GetParams(const EVariation InVariationName);

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
private:
	UPROPERTY(EditAnywhere, Category = "Vfx Data", meta = (AllowPrivateAccess = true))
	FName VfxPrimaryName;
	UPROPERTY(EditAnywhere, Category = "Vfx", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UNiagaraSystem> VfxEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx Data", meta = (AllowPrivateAccess = true))
	TMap<EVariation, FVariationVfxParams> VariationParams;
};
