#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "PoisonArea.generated.h"

UCLASS()
class POSSESSIONGAME_API APoisonArea : public AOvertimeDamager
{
	GENERATED_BODY()

	bool bIsActionSpeedReduceUpgradeTaken = false;

	virtual void Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason) override;
	virtual void DealOvertimeDamage() override;
	virtual void AddTarget(const TObjectPtr<AEnemy>& InTarget) override;
	virtual void RemoveTarget(const TObjectPtr<AEnemy>& InTarget) override;
};
