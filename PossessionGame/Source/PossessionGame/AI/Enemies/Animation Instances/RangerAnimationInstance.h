#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "RangerAnimationInstance.generated.h"

enum class EVfx : uint8;

UCLASS()
class POSSESSIONGAME_API URangerAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	UPROPERTY()
	FTimerHandle TriggerGrenadeProjectionHandle;
	UPROPERTY()
	FTimerHandle LaunchGrenadeHandle;

	virtual void InnerPlayMontage(const EEnemyAnimState InState) override;
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
	void LaunchGrenade() const;
	void TriggerGrenadeProjection() const;
	virtual void ClearTimers() override;
};
