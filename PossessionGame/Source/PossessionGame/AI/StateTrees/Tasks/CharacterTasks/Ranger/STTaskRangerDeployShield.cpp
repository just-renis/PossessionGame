#include "STTaskRangerDeployShield.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/RangerAIController.h"

EStateTreeRunStatus USTTaskRangerDeployShield::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	const TObjectPtr<ARangerAIController> CachedController = Cast<ARangerAIController>(Context.GetOwner());
	CachedController->GetProprietor()->AI_UseAbility(1);
	CachedController->OnDeployShieldFinished.BindUObject(this, &USTTaskRangerDeployShield::DeployShieldFinished);
	return EStateTreeRunStatus::Running;
}

void USTTaskRangerDeployShield::DeployShieldFinished() { FinishTask(); }
