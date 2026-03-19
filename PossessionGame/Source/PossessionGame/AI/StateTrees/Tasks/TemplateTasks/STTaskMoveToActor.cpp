#include "STTaskMoveToActor.h"
#include "StateTreeExecutionContext.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Navigation/PathFollowingComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

EStateTreeRunStatus USTTaskMoveToActor::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<AEnemyAIController>(Context.GetOwner());
	const EStateTreeRunStatus Status = PathFollowingRequestResultHandling(CachedController->MoveTo(SetupAndGetMoveReq()));
	if (Status != EStateTreeRunStatus::Running) return Status;
	if (!CachedController->GetEqsTargetUpdateDelegate().IsBound() && bMoveModeSwitchEnabled) CachedController->GetEqsTargetUpdateDelegate().BindUObject(this, &USTTaskMoveToActor::SetMoveMode);
	CachedController->SetMoveMode(MoveMode);
	CachedController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &USTTaskMoveToActor::OnMoveCompleted);
	return Super::EnterState(Context, Transition);
}

void USTTaskMoveToActor::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController->StopMovement();
	CachedController->SetMoveMode(EMoveMode::Walk);
	CachedController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
	GetWorld()->GetTimerManager().ClearTimer(CheckForNewMoveReqHandle);
	Super::ExitState(Context, Transition);
}

bool USTTaskMoveToActor::DistanceTestCondition(const float InDistance) const
{
	switch (Operator)
	{
		case EGenericAICheck::Less: return InDistance < Distance;
		case EGenericAICheck::LessOrEqual: return InDistance <= Distance;
		case EGenericAICheck::Equal: return FMath::IsNearlyEqual(InDistance, Distance, 0.001f);
		case EGenericAICheck::NotEqual: return !FMath::IsNearlyEqual(InDistance, Distance, 0.001f);
		case EGenericAICheck::GreaterOrEqual: return InDistance >= Distance;
		case EGenericAICheck::Greater: return InDistance > Distance;
		case EGenericAICheck::IsTrue: return Distance != 0.f;
		default: return false;
	}
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

EMoveMode USTTaskMoveToActor::ReceiveMoveMode() const
{
	const FVector ActorLocation = CachedController->GetPawn()->GetActorLocation();
	const FVector TargetActorLocation = Target->GetActorLocation();
	const bool Result = bCheckWithTargetObject ? DistanceTestCondition(FVector::Dist(ActorLocation, TargetActorLocation)) : DistanceTestCondition(FVector::Dist(ActorLocation, TargetLocation));
	return bInvert ? (Result ? EMoveMode::Walk : EMoveMode::Run) : Result ? EMoveMode::Run : EMoveMode::Walk;
}

void USTTaskMoveToActor::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	CachedController->GetStateTreeData().SetEQSPointAvailability(false);
	if (!CachedController->GetIsUsingEQS()) CachedController->ResetOnReachedNonEqsPoint();
	if (bFinishTaskOnMoveCompleted)
	{
		FinishTask();
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(CheckForNewMoveReqHandle, this, &USTTaskMoveToActor::CheckForNewMoveReq, 0.01f, true);
}

void USTTaskMoveToActor::CheckForNewMoveReq()
{
	if (!CachedController->GetStateTreeData().GetIsEQSPointAvailable()) return;
	PathFollowingRequestResultHandling(CachedController->MoveTo(SetupAndGetMoveReq()));
	if (bMoveModeSwitchEnabled) MoveMode = ReceiveMoveMode();
	CachedController->SetMoveMode(MoveMode);
	GetWorld()->GetTimerManager().ClearTimer(CheckForNewMoveReqHandle);
}

void USTTaskMoveToActor::SetMoveMode()
{
	MoveMode = ReceiveMoveMode();
	CachedController->SetMoveMode(MoveMode);
}
