#include "SamuraiAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
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
			if (FMath::IsNearlyEqual(EQSTarget->GetActorLocation().Z, Proprietor->GetActorLocation().Z, 5.f))
			{
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

void ASamuraiAIController::CheckDistanceBetweenTarget()
{
	if (!EQSTarget || !Proprietor || Proprietor->GetIsAnimActionInMotion()) return;
	CheckIfStuck();
	const float Distance = FVector::DistSquared(Proprietor->GetActorLocation(), EQSTarget->GetActorLocation());
	if (Distance <= FMath::Square(DistanceBeforeSlashingInCentimetres))
	{
		Proprietor->SetIsAnimActionInMotion(true);
		Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Slash);
	}
}

void ASamuraiAIController::CheckIfStuck()
{
	const FVector Direction = EQSTarget->GetActorLocation() - Proprietor->GetActorLocation();
	const bool bIsNearPlayer = Direction.SizeSquared() <= DistanceBeforeSlashingInCentimetres * DistanceBeforeSlashingInCentimetres;
	if (!bIsNearPlayer && Proprietor->GetVelocity().Size() < 10.f)
	{
		StuckTimer += 0.1f;
		if (StuckTimer >= StuckThreshold)
		{
			StopStateTreeLogic(false);
			Proprietor->GetCharacterMovement()->Velocity += Direction.GetSafeNormal2D() * 500.f;
			Proprietor->Jump();
			bIsInJumpMode = true;
			StuckTimer = 0.f;
		}
	}
	else StuckTimer = 0.f;
}

void ASamuraiAIController::SetIsInJumpMode(const bool bState)
{
	Super::SetIsInJumpMode(bState);
	ResumeLogic();
}
