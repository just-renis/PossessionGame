#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "RangerAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API URangerAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	FTimerHandle TriggerGrenadeProjectionHandle;
	FTimerHandle LaunchGrenadeHandle;

	virtual void InnerPlayMontage(const EEnemyAnimState InState) override;
	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
	void LaunchGrenade() const;
	void TriggerGrenadeProjection() const;
	virtual void ClearTimers() override;
};
