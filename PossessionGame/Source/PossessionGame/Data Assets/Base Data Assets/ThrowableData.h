#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ThrowableData.generated.h"

UCLASS()
class POSSESSIONGAME_API UThrowableData : public UDataAsset
{
	GENERATED_BODY()
public:
	bool GetNeedsOnHitEvent() const { return bNeedsOnHitEvent; }
	float GetDamage() const { return Damage; }
	float GetRadiusMinimumDamage() const { return RadiusMinimumDamage; }
	float GetDamageInnerRadius() const { return DamageInnerRadius; }
	float GetDamageOuterRadius() const { return DamageOuterRadius; }
	float GetDamageFalloff() const { return DamageFalloff; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool bNeedsOnHitEvent = false;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Damage = 0.f;
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
};
