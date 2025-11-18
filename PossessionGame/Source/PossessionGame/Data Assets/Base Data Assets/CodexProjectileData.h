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
	bool bProjectileHasEffectDuration = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileHasEffectDuration", EditConditionHides))
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
