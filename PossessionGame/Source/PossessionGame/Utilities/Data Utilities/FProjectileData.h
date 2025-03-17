#pragma once

#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "FProjectileData.generated.h"

class AEnemy;

USTRUCT()
struct POSSESSIONGAME_API FProjectileData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bIsPlayerProjectile = false;
	UPROPERTY(EditAnywhere)
	float LifeSpan = 0.f;
	UPROPERTY(EditAnywhere)
	float Damage = 0.f;
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 2000.f;
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 2000.f;
	UPROPERTY(EditAnywhere)
	float GravityScale = 0.f;
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
	EPoolObject ProjectileName = EPoolObject::None;
	
	float GetCalcSpread() const { return Spread * (bWalkSpreadEnabled ? 2.f : 1.f); }
	TObjectPtr<AEnemy> GetProprietor() { return Proprietor; }

	void SetWalkSpread(bool bState) { bWalkSpreadEnabled = bState; }
	void SetProprietor(const TObjectPtr<AEnemy>& InProprietor) { Proprietor = InProprietor; }
private:
	bool bWalkSpreadEnabled = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float Spread = 0.f;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
};
