#include "STConditionDistanceCompare.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

bool USTConditionDistanceCompare::TestCondition(FStateTreeExecutionContext& Context) const
{
	bool Result = bCheckWithTargetObject ? ConditionResult(FVector::Dist(Cast<AEnemyAIController>(Context.GetOwner())->GetPawn()->GetActorLocation(), Target->GetActorLocation())) :
	ConditionResult(FVector::Dist(Cast<AEnemyAIController>(Context.GetOwner())->GetPawn()->GetActorLocation(), TargetLocation));
	return bInvert ? !Result : Result;
}

bool USTConditionDistanceCompare::ConditionResult(float DistanceBetweenLocations) const
{
	switch (Operator)
	{
		case EGenericAICheck::Less:
			return DistanceBetweenLocations < Distance;
		case EGenericAICheck::LessOrEqual:
			return DistanceBetweenLocations <= Distance;
		case EGenericAICheck::Equal:
			return FMath::IsNearlyEqual(DistanceBetweenLocations, Distance, 0.001f);
		case EGenericAICheck::NotEqual:
			return !FMath::IsNearlyEqual(DistanceBetweenLocations, Distance, 0.001f);
		case EGenericAICheck::GreaterOrEqual:
			return DistanceBetweenLocations >= Distance;
		case EGenericAICheck::Greater:
			return DistanceBetweenLocations > Distance;
		case EGenericAICheck::IsTrue:
			return Distance != 0.f;
		default:
			return false;
	}
}
