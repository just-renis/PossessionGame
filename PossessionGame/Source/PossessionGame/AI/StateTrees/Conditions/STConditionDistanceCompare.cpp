#include "STConditionDistanceCompare.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

bool USTConditionDistanceCompare::TestCondition(FStateTreeExecutionContext& Context) const
{
	const TObjectPtr<AEnemyAIController> Controller = Cast<AEnemyAIController>(Context.GetOwner());
	if (!Controller || !Controller->GetPawn()) return false;
	const FVector ActorLocation = Controller->GetPawn()->GetActorLocation();
	const FVector TargetActorLocation = Target->GetActorLocation();
	const bool Result = bCheckWithTargetObject ? ConditionResult(FVector::Dist(ActorLocation, TargetActorLocation)) : ConditionResult(FVector::Dist(ActorLocation, TargetLocation));
	return bInvert ? !Result : Result;
}

bool USTConditionDistanceCompare::ConditionResult(const float InDistanceBetweenLocations) const
{
	switch (Operator)
	{
		case EGenericAICheck::Less:
			return InDistanceBetweenLocations < Distance;
		case EGenericAICheck::LessOrEqual:
			return InDistanceBetweenLocations <= Distance;
		case EGenericAICheck::Equal:
			return FMath::IsNearlyEqual(InDistanceBetweenLocations, Distance, 0.001f);
		case EGenericAICheck::NotEqual:
			return !FMath::IsNearlyEqual(InDistanceBetweenLocations, Distance, 0.001f);
		case EGenericAICheck::GreaterOrEqual:
			return InDistanceBetweenLocations >= Distance;
		case EGenericAICheck::Greater:
			return InDistanceBetweenLocations > Distance;
		case EGenericAICheck::IsTrue:
			return Distance != 0.f;
		default:
			return false;
	}
}
