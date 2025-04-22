#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskMinionBossLaserEye.generated.h"

UCLASS(DisplayName = "Laser Eye", Category = "Character Tasks | Minion Boss Tasks")
class POSSESSIONGAME_API USTTaskMinionBossLaserEye : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	UFUNCTION()
	void LaserEyeFinished();
};
