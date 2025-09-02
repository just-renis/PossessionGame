#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskMinionBossLaserEye.generated.h"

class AMinionBossAIController;

UCLASS(DisplayName = "Laser Eye", Category = "Character Tasks | Minion Boss Tasks")
class POSSESSIONGAME_API USTTaskMinionBossLaserEye : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<AMinionBossAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void LaserEyeFinished();
};
