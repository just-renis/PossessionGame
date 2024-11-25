#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "SamuraiAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API USamuraiAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void BeginDestroy() override;
	virtual void AssignOnMontageEnd() override;
	UFUNCTION()
	void OnSamuraiMontageEnd(UAnimMontage* Montage, bool bIsInterrupted);
};
