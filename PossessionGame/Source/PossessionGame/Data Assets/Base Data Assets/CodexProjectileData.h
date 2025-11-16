#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CodexProjectileData.generated.h"

UCLASS()
class POSSESSIONGAME_API UCodexProjectileData : public UDataAsset
{
	GENERATED_BODY()
public:
	int32 GetAmount() const { return Amount; }
	float GetDamage() const { return Damage; }
	float GetOvertimeAreaLifeSpan() const { return OvertimeAreaLifeSpan; }
	float GetOvertimeAreaSize() const { return OvertimeAreaSize; }
	float GetEffectDuration() const { return EffectDuration; }
	float GetSpeed() const { return Speed; }
	float GetDamageInnerRadius() const { return DamageInnerRadius; }
	float GetDamageOuterRadius() const { return DamageOuterRadius; }
private:
	UPROPERTY(EditAnywhere)
	int32 Amount = 1;
	UPROPERTY(EditAnywhere)
	float Damage = 0.f;
	UPROPERTY(EditAnywhere)
	bool bProjectileDealsOvertimeDamage = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsOvertimeDamage", EditConditionHides))
	float OvertimeAreaLifeSpan = 0.f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsOvertimeDamage", EditConditionHides))
	float OvertimeAreaSize = 0.f;
	UPROPERTY(EditAnywhere)
	bool bProjectileAppliesEffect = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileAppliesEffect", EditConditionHides))
	float EffectDuration = 0.f;
	UPROPERTY(EditAnywhere)
	bool bProjectileDealsAreaDamage = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsAreaDamage", EditConditionHides))
	float DamageInnerRadius = 0.f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsAreaDamage", EditConditionHides))
	float DamageOuterRadius = 0.f;
	UPROPERTY(EditAnywhere)
	float Speed = 2000.f;
};
