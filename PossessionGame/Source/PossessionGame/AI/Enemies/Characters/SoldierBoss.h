#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "SoldierBoss.generated.h"

UCLASS()
class POSSESSIONGAME_API ASoldierBoss : public AEnemy
{
	GENERATED_BODY()

	ASoldierBoss();

	virtual void AI_SetupWeapon() override;
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;

	virtual void BeginPlay() override;
	virtual void ApplyEmpEffect(const float InEffectDuration) override;
	virtual void ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent) override;
	virtual void InitiateDeath() override;
};
