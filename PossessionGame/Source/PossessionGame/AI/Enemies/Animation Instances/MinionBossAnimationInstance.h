#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "MinionBossAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UMinionBossAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
};
