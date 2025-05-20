#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "RangerAnimationInstance.generated.h"

enum class EVfx : uint8;

UCLASS()
class POSSESSIONGAME_API URangerAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void InnerPlayMontage(const EEnemyAnimState InState, const bool bPlayInReverse) override;
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
	UFUNCTION()
	void LaunchGrenade() const;
	UFUNCTION()
	void TriggerGrenadeProjection() const;
};
