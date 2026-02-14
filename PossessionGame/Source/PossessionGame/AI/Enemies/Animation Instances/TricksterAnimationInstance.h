#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "TricksterAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UTricksterAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
};
