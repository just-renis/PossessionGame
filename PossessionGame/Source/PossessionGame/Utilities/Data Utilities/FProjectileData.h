#pragma once

#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "FProjectileData.generated.h"

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

	UPROPERTY(EditAnywhere)
	bool bIsPlayerProjectile = false;
	UPROPERTY(EditAnywhere)
	bool bIsProjectileDealingDamageItself = true;
	UPROPERTY(EditAnywhere)
	float LifeSpan = 0.f;
	UPROPERTY(EditAnywhere)
	float Damage = 0.f;
	UPROPERTY(EditAnywhere)
	bool bProjectileDealsOvertimeDamage = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bProjectileDealsOvertimeDamage", EditConditionHides))
	float OvertimeTickTrigger = 0.f;
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
	EPoolObject ProjectileName = EPoolObject::None;
	int32 Amount = 1;
	FAdditionalProjectiles GetMultiProjectileData(const int32 InAmount)
	{
		if (MultipleProjectiles.Find(InAmount)) return MultipleProjectiles[InAmount];
		return FAdditionalProjectiles();
	}
	float GetCalcSpread() const { return Spread * (bWalkSpreadEnabled ? 2.f : 1.f); }
	
	TObjectPtr<AEnemy> GetProprietor() { return Proprietor; }

	void SetWalkSpread(bool bState) { bWalkSpreadEnabled = bState; }
	void SetProprietor(const TObjectPtr<AEnemy>& InProprietor) { Proprietor = InProprietor; }
	void SetAmount(const int32 InAmount) { Amount = InAmount; }
private:
	bool bWalkSpreadEnabled = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<int32, FAdditionalProjectiles> MultipleProjectiles;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float Spread = 0.f;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
};
