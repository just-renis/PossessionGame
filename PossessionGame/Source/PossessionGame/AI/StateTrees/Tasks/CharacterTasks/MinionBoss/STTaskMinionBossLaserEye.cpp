#include "STTaskMinionBossLaserEye.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/MinionBossAIController.h"

EStateTreeRunStatus USTTaskMinionBossLaserEye::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	const TObjectPtr<AMinionBossAIController> CachedController = Cast<AMinionBossAIController>(Context.GetOwner());
	CachedController->GetProprietor()->AI_UseAbility(1);
	CachedController->OnLaserEyeFinished.BindUObject(this, &USTTaskMinionBossLaserEye::LaserEyeFinished);
	return EStateTreeRunStatus::Running;
}

void USTTaskMinionBossLaserEye::LaserEyeFinished() { FinishTask(); }
