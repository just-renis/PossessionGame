#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "WraithAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UWraithAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void MontageWithHoldEnd() override;
};
