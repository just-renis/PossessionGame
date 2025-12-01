#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "BreacherAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UBreacherAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
};
