#include "STTaskMinionBossMoveDirectlyToPlayer.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

EStateTreeRunStatus USTTaskMinionBossMoveDirectlyToPlayer::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	const EStateTreeRunStatus TreeRunStatus = Super::EnterState(Context, Transition);
	if (TreeRunStatus != EStateTreeRunStatus::Running) return TreeRunStatus;
	GetWorld()->GetTimerManager().SetTimer(MoveDirectlyToPlayerHandle, this, &USTTaskMinionBossMoveDirectlyToPlayer::OnMoveDirectlyDurationFinished, MoveDirectlyToPlayerDuration);
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::MinionBossMoveDirectlyToPlayer);
	CachedController->GetProprietor()->SetMovementSpeedMultiplier(MoveSpeedMultiplier);
	return TreeRunStatus;
}

void USTTaskMinionBossMoveDirectlyToPlayer::OnMoveDirectlyDurationFinished()
{
	Cast<AMinionBoss>(CachedController->GetProprietor())->ResetMoveDirectlyToPlayer();
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
