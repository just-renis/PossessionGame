#include "SamuraiAIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

void ASamuraiAIController::BeginPlay()
{
	Super::BeginPlay();
	bIsControlRigAvailableToToggle = false;
	SetIsUsingEQS(false);
	GetWorldTimerManager().SetTimer(CheckDistanceBetweenTargetHandle, this, &ASamuraiAIController::CheckDistanceBetweenTarget, 0.1f, true);
	EQSTarget = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetTarget();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnHijack.AddUObject(this, &ASamuraiAIController::SetPlayerTarget);
}

void ASamuraiAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnHijack.RemoveAll(this);
	GetWorldTimerManager().ClearTimer(CheckDistanceBetweenTargetHandle);
	Super::EndPlay(EndPlayReason);
}

void ASamuraiAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		GetWorld()->GetTimerManager().ClearTimer(OnLostSightOfPawnHandle);
		if (!StateTreeData.GetIsTargetInMySight())
		{
			//UE_LOG(LogTemp, Error, TEXT("%s PLAYER: %f, ENEMY: %f"), *Proprietor->GetName(), EQSTarget->GetActorLocation().Z, Proprietor->GetActorLocation().Z);
			if (FMath::IsNearlyEqual(EQSTarget->GetActorLocation().Z, Proprietor->GetActorLocation().Z, 5.f))
			{
				UE_LOG(LogTemp, Error, TEXT("MOVE DIRECTLY %s"), *Proprietor->GetName());
				bIsMovingDirectly = true;
				GetStateTreeData().TriggerEvent(EStateTreeEvent::MoveDirectly);
			}
			Proprietor->GetAnimInstance()->SetIsTargetInSight(true);
			SetIsTargetInMySight(true);
		}
	}
	else GetWorld()->GetTimerManager().SetTimer(OnLostSightOfPawnHandle, this, &ASamuraiAIController::OnLostSightOfPawn, 0.3f, false);
}

void ASamuraiAIController::OnLostSightOfPawn()
{
	if (bIsMovingDirectly)
	{
			UE_LOG(LogTemp, Error, TEXT("PATHFINDER %s"), *Proprietor->GetName());
		GetStateTreeData().TriggerEvent(EStateTreeEvent::MoveWithPathfinder);
		bIsMovingDirectly = false;
	}
	Super::OnLostSightOfPawn();
}

void ASamuraiAIController::SetPlayerTarget(const TObjectPtr<AActor>& InPlayerTarget)
{
	EQSTarget = InPlayerTarget;
	GetStateTreeData().SetEQSTargetRef(EQSTarget);
	GetStateTreeData().TriggerEvent(EStateTreeEvent::MoveWithPathfinder);
}

void ASamuraiAIController::CheckDistanceBetweenTarget() const
{
	if (!EQSTarget || !Proprietor || Proprietor->GetIsActionInMotion()) return;
	const float Distance = FVector::DistSquared(Proprietor->GetActorLocation(), EQSTarget->GetActorLocation());
	if (Distance <= FMath::Square(DistanceBeforeSlashingInCentimetres))
	{
		Proprietor->SetIsActionInMotion(true);
		Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Slash);
	}
}
