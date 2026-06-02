#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PossessionGame/Utilities/Data Utilities/FVariationVfxParams.h"
#include "ThrowableData.generated.h"

enum class EVariation : uint8;
class UDamageOverTimeData;

UCLASS()
class POSSESSIONGAME_API UThrowableData : public UDataAsset
{
	GENERATED_BODY()
public:
	bool IsVariationValuesExist() const { return !VariationValues.IsEmpty(); }
	bool GetNeedsOnHitEvent() const { return bNeedsOnHitEvent; }
	int32 GetCap() const { return Cap; }
	float GetDamage() const { return Damage; }
	float GetImpulseForce() const { return ImpulseForce; }
	float GetRadiusMinimumDamage() const { return RadiusMinimumDamage; }
	float GetDamageInnerRadius() const { return DamageInnerRadius; }
	float GetDamageOuterRadius() const { return DamageOuterRadius; }
	float GetDamageFalloff() const { return DamageFalloff; }
	float GetDirectionMultiplier() const { return DirectionMultiplier; }
	TObjectPtr<UDamageOverTimeData> GetDamageOverTimeData() const { return DamageOverTimeData; }
	
    void SetupVariationMaterial(const TObjectPtr<UMaterialInstanceDynamic>& InMaterial, const EVariation InVariation);
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool bNeedsOnHitEvent = false;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 Cap = 3;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Damage = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float ImpulseForce = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float DirectionMultiplier = 0.3f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool bIsDealingRadiusDamage = false;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bIsDealingRadiusDamage", EditConditionHides, AllowPrivateAccess = true))
	float RadiusMinimumDamage = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bIsDealingRadiusDamage", EditConditionHides, AllowPrivateAccess = true))
	float DamageInnerRadius = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bIsDealingRadiusDamage", EditConditionHides, AllowPrivateAccess = true))
	float DamageOuterRadius = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bIsDealingRadiusDamage", EditConditionHides, AllowPrivateAccess = true))
	float DamageFalloff = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool bHasDamageOverTime = false;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bHasDamageOverTime", EditConditionHides, AllowPrivateAccess = true))
	TObjectPtr<UDamageOverTimeData> DamageOverTimeData;
	UPROPERTY(EditDefaultsOnly, Category = "Variation Values", meta = (AllowPrivateAccess = true))
	TMap<EVariation, FVariationVfxParams> VariationValues;
};
