#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "SoldierBossAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API USoldierBossAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void MontageWithHoldEnd() override;
};
