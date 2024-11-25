#include "STTaskRotateToLocation.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

EStateTreeRunStatus USTTaskRotateToLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	TObjectPtr<AEnemyAIController> Controller = Cast<AEnemyAIController>(Context.GetOwner());
	//FRotator NewControlRotation = Controller->GetControlRotation();
	//NewControlRotation.Yaw = (TargetLocation - Controller->GetPawn()->GetActorLocation()).GetSafeNormal2D().Rotation().Yaw;
	//Controller->GetPawn()->SetActorRotation(NewControlRotation);
	if (Target)
	{
		FVector DirectionToTarget = Target->GetActorLocation() - Controller->GetPawn()->GetActorLocation();
		DirectionToTarget.Z = 0;
		DirectionToTarget.Normalize();
		FRotator CurrentRotation = Controller->GetPawn()->GetActorRotation();
		Controller->GetPawn()->SetActorRotation(FRotator(CurrentRotation.Pitch, DirectionToTarget.Rotation().Yaw, CurrentRotation.Roll));
	}
	return EStateTreeRunStatus::Succeeded;
}
