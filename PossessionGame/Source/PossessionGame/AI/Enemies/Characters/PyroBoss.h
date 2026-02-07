#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PyroBoss.generated.h"

UCLASS()
class POSSESSIONGAME_API APyroBoss : public AEnemy
{
	GENERATED_BODY()
public:
	void MarkFloorLavaAbilityReady();
private:
	FTimerHandle ActivateFloorLavaHandle;
	
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;
	virtual void AI_SetupWeapon() override;

	virtual void BeginPlay() override;
	virtual void InitiateDeath() override;
	virtual void ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent) override;
	virtual void ApplyEmpEffect(const float InEffectDuration) override;
	void ActivateFloorLava();
};
