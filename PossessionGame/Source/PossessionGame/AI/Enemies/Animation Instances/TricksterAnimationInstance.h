#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "TricksterAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UTricksterAnimationInstance : public UEnemyAnimationInstance
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
