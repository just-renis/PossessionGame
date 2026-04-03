#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PyroBossAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UPyroBossAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
	virtual void MontageWithHoldEnd() override;
};
