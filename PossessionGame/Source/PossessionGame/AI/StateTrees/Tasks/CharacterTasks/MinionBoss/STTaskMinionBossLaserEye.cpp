#include "STTaskMinionBossLaserEye.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/MinionBossAIController.h"

EStateTreeRunStatus USTTaskMinionBossLaserEye::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<AMinionBossAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::MinionBossLaserEye);
	CachedController->GetProprietor()->AI_UseAbility(1);
	CachedController->OnLaserEyeFinished.BindUObject(this, &USTTaskMinionBossLaserEye::LaserEyeFinished);;
	return Super::EnterState(Context, Transition);
}

void USTTaskMinionBossLaserEye::LaserEyeFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
