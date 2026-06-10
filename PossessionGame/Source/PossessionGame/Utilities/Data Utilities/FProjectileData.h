#pragma once

#include "FVariationVfxParams.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "FProjectileData.generated.h"

class UDamageOverTimeData;
class AEnemy;

USTRUCT()
struct FAdditionalProjectileData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 2000.f;
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 2000.f;
	UPROPERTY(EditAnywhere)
	float GravityScale = 0.f;
	UPROPERTY(EditAnywhere)
	FVector Direction = FVector::ZeroVector;
};

USTRUCT()
struct FAdditionalProjectiles
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TArray<FAdditionalProjectileData> Data;
};

USTRUCT()
struct POSSESSIONGAME_API FProjectileData
{
	GENERATED_BODY()

	int32 Amount = 1;
	UPROPERTY(EditAnywhere)
	bool bIsPlayerProjectile = false;
	UPROPERTY(EditAnywhere)
	bool bIsImpactOnPlayerVfxEnabled = false;
	UPROPERTY(EditAnywhere)
	bool bIsProjectileDealingDamageItself = true;
	UPROPERTY(EditAnywhere)
	float LifeSpan = 0.f;
	UPROPERTY(EditAnywhere)
	float Damage = 0.f;
	UPROPERTY(EditAnywhere)
	bool bIsShotByWeapon = true;
	UPROPERTY(EditAnywhere)
	bool bProjectileDealsOvertimeDamage = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsOvertimeDamage", EditConditionHides))
	TObjectPtr<UDamageOverTimeData> DamageOverTimeData;
	UPROPERTY(EditAnywhere)
	bool bProjectileAppliesEffect = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileAppliesEffect", EditConditionHides))
	float EffectDuration = 0.f;
	UPROPERTY(EditAnywhere)
	bool bProjectileDealsAreaDamage = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsAreaDamage", EditConditionHides))
	bool bCanBeDamagedByOwnProjectile = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsAreaDamage", EditConditionHides))
	float RadiusMinimumDamage = 0.f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsAreaDamage", EditConditionHides))
	float DamageInnerRadius = 0.f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsAreaDamage", EditConditionHides))
	float DamageOuterRadius = 0.f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsAreaDamage", EditConditionHides))
	float DamageFalloff = 0.f;
	UPROPERTY(EditAnywhere)
	FName ProjectileSocketName = "ProjectileSocket";
	UPROPERTY(EditDefaultsOnly)
	EDamageReason DamageReason = EDamageReason::None;
	UPROPERTY(EditDefaultsOnly)
	EPoolObject ProjectileName = EPoolObject::None;
	UPROPERTY(EditDefaultsOnly, Category = "Variation Values", meta = (AllowPrivateAccess = true))
	TMap<EVariation, FMaterialVariationVfxParams> VariationValues;

	bool IsVariationValuesExist() const { return !VariationValues.IsEmpty(); }
	float GetCalcSpread() const { return Spread * (bWalkSpreadEnabled ? 2.f : 1.f); }
	FAdditionalProjectiles GetMultiProjectileData(const int32 InAmount)
	{
		if (FAdditionalProjectiles* ProjectileData = MultipleProjectiles.Find(InAmount)) return *ProjectileData;
		return FAdditionalProjectiles();
	}
	FAdditionalProjectiles* GetMultiProjectileDataPtr(const int32 InAmount)
	{
		if (FAdditionalProjectiles* Found = MultipleProjectiles.Find(InAmount)) return Found;
		return nullptr;
	}
	TObjectPtr<AEnemy> GetProprietor() { return Proprietor; }

	void SetWalkSpread(const bool bState) { bWalkSpreadEnabled = bState; }
	void SetProprietor(const TObjectPtr<AEnemy>& InProprietor) { Proprietor = InProprietor; }
	void SetAmount(const int32 InAmount) { Amount = InAmount; }
    void SetupVariationMaterials(const TArray<TObjectPtr<UMaterialInstanceDynamic>>& InMaterials)
	{
		if (!Proprietor) return;
		const FMaterialVariationVfxParams* VariationData = VariationValues.Find(Proprietor->GetVariation());
		if (!VariationData) return;
		for (const TPair<int32, FVariationVfxParams>& MaterialPair : VariationData->Params)
		{
			const int32 MaterialIndex = MaterialPair.Key;
			if (!InMaterials.IsValidIndex(MaterialIndex)) continue;
			UMaterialInstanceDynamic* Material = InMaterials[MaterialIndex];
			for (const TPair<FName, FNiagaraUserParam>& Param : MaterialPair.Value.Params)
			{
				switch (Param.Value.ParamType)
				{
				case EUserParamType::Bool: Material->SetScalarParameterValue(Param.Key, Param.Value.bBoolValue); break;
				case EUserParamType::Int: Material->SetScalarParameterValue(Param.Key, Param.Value.IntValue); break;
				case EUserParamType::Float: Material->SetScalarParameterValue(Param.Key, Param.Value.FloatValue); break;
				case EUserParamType::Vector: Material->SetVectorParameterValue(Param.Key, Param.Value.VectorValue); break;
				case EUserParamType::Color: Material->SetVectorParameterValue(Param.Key, Param.Value.ColorValue); break;
				case EUserParamType::Texture: Material->SetTextureParameterValue(Param.Key, Param.Value.Texture); break;
				default: break;
				}
			}
		}
	}
private:
	bool bWalkSpreadEnabled = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<int32, FAdditionalProjectiles> MultipleProjectiles;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float Spread = 0.f;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
};
