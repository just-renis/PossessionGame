#include "STTaskGetRandomEQSPoint.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

EStateTreeRunStatus USTTaskGetRandomEQSPoint::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	TObjectPtr<AEnemyAIController> Controller = Cast<AEnemyAIController>(Context.GetOwner());
	Controller->SetEQSTargetLocation(GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetPointLocation(Controller->GetProprietor()->GetCharacterName(), Controller->GetProprietor()->GetActorLocation()));
	return EStateTreeRunStatus::Succeeded;
}
