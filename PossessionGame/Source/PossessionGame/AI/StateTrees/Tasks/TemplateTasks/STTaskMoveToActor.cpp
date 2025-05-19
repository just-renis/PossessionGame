#include "STTaskMoveToActor.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Navigation/PathFollowingComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

EStateTreeRunStatus USTTaskMoveToActor::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<AEnemyAIController>(Context.GetOwner());
	const EStateTreeRunStatus Status = PathFollowingRequestResultHandling(CachedController->MoveTo(SetupAndGetMoveReq()));
	if (Status != EStateTreeRunStatus::Running) return Status;
	CachedController->SetMoveMode(MoveMode);
	CachedController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &USTTaskMoveToActor::OnMoveCompleted);
	return Super::EnterState(Context, Transition);
}

void USTTaskMoveToActor::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController->StopMovement();
	CachedController->SetMoveMode(EMoveMode::Walk);
	CachedController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
	Super::ExitState(Context, Transition);
}

EStateTreeRunStatus USTTaskMoveToActor::PathFollowingRequestResultHandling(EPathFollowingRequestResult::Type MoveResult)
{
	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal) return EStateTreeRunStatus::Succeeded;
	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
	//	UE_LOG(LogTemp, Error, TEXT("Failed to start movement"));
		return EStateTreeRunStatus::Failed;
	}
	return EStateTreeRunStatus::Running;
}

FAIMoveRequest USTTaskMoveToActor::SetupAndGetMoveReq()
{
	FAIMoveRequest MoveReq;
	if (bMoveToTargetLocation)
	{
		if (TargetLocation.ContainsNaN()) MoveReq = FAIMoveRequest(Target->GetActorLocation());
		else MoveReq = FAIMoveRequest(TargetLocation);
	}
	else MoveReq = FAIMoveRequest(Target);
	MoveReq.SetUsePathfinding(bPathFinding);
	MoveReq.SetAllowPartialPath(bAllowPartial);
	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : CachedController->GetDefaultNavigationFilterClass());
	MoveReq.SetAcceptanceRadius(AcceptableRadius);
	MoveReq.SetReachTestIncludesAgentRadius(false);
	MoveReq.SetReachTestIncludesGoalRadius(false);
	MoveReq.SetCanStrafe(bStrafe);
	return MoveReq;
}

void USTTaskMoveToActor::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	CachedController->GetStateTreeData().SetEQSPointAvailability(false);
	if (bFinishTaskOnMoveCompleted)
	{
		FinishTask();
		return;
	}
	UKismetSystemLibrary::K2_SetTimer(this, "CheckForNewMoveReq", 0.01f, true);
}

void USTTaskMoveToActor::CheckForNewMoveReq()
{
	if (!CachedController->GetStateTreeData().GetIsEQSPointAvailable()) return;
	PathFollowingRequestResultHandling(CachedController->MoveTo(SetupAndGetMoveReq()));
	CachedController->SetMoveMode(MoveMode);
	UKismetSystemLibrary::K2_ClearTimer(this, "CheckForNewMoveReq");
}
