#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "RangerAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API URangerAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void InnerPlayMontage(EEnemyAnimState State) override;
	//virtual void SetProprietor() override;
	UFUNCTION()
	void LaunchGrenade();
};
