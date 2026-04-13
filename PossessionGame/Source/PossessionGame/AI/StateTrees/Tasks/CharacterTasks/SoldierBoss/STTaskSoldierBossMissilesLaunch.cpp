#include "STTaskSoldierBossMissilesLaunch.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/SoldierBossAIController.h"

EStateTreeRunStatus USTTaskSoldierBossMissilesLaunch::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<ASoldierBossAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::SoldierBossMissilesLaunch);
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnMissilesLaunchFinished.BindUObject(this, &USTTaskSoldierBossMissilesLaunch::MissilesLaunchFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskSoldierBossMissilesLaunch::MissilesLaunchFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
