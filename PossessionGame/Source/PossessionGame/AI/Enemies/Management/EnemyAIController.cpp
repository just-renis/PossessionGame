#include "EnemyAIController.h"
#include "Enemy.h"
#include "EnemyAnimationInstance.h"
#include "Components/StateTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/EmptyActor.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.TickInterval = 0.1f;
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("perceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("sightConfig"));
	SightConfig->SightRadius = 6000.f;
	SightConfig->LoseSightRadius = 6000.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	Perception->ConfigureSense(*SightConfig);
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	StateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTree->SetStartLogicAutomatically(false);
	StateTree->SetComponentTickInterval(0.1f);
}

void AEnemyAIController::SetShootCooldown(const float InValue) { StateTreeData.SetShootCooldown(InValue); }

void AEnemyAIController::SetReloadDelay(const float InValue) { StateTreeData.SetReloadDelay(InValue); }

void AEnemyAIController::DestroyThis()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTargetChange.RemoveAll(this);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnPlayerDeath.RemoveAll(this);
	if (Perception)
	{
		Perception->Deactivate();
		Perception->OnPerceptionUpdated.Clear();
	}
	EnemyCommunicator->GetEQSDelegate().RemoveAll(this);
	StopStateTreeLogic(true);
}

void AEnemyAIController::SetProprietor(const TObjectPtr<AEnemy>& InEnemy) { Proprietor = InEnemy; }

void AEnemyAIController::SetupStateTree()
{
	if (StateTree)
	{
		StateTreeData.SetWeaponRef(Proprietor->GetWeapon());
		StateTreeData.SetEQSTargetRef(EQSTarget);
		StateTreeData.SetStateTreeRef(StateTree);
		StateTree->StartLogic();
	}
}

void AEnemyAIController::SetMoveMode(const EMoveMode InMoveMode) const { Proprietor->AI_SetMoveMode(InMoveMode); }

void AEnemyAIController::SetupAI()
{
	GetWorld()->GetTimerManager().SetTimer(AdjustCustomFocusHandle, this, &AEnemyAIController::AdjustCustomFocus, 0.01f, true);
	EnemyCommunicator->GetEQSDelegate().AddUObject(this, &AEnemyAIController::SetEQSTargetLocation);
	if (EnemyCommunicator->GetTarget()) SetTarget(EnemyCommunicator->GetTarget(), false);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTargetChange.AddUObject(this, &AEnemyAIController::SetTarget);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnPlayerDeath.AddUObject(this, &AEnemyAIController::StopStateTreeLogic);
}

void AEnemyAIController::ResumeLogic() const { if (StateTree) StateTree->StartLogic(); }

void AEnemyAIController::StopLogic(const bool bNeedCleanup)
{
	StopMovement();
	StopStateTreeLogic(bNeedCleanup);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	EnemyCommunicator = GetWorld()->GetSubsystem<UEnemyCommunicator>();
	SetupEQSTarget();
}

void AEnemyAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(AdjustCustomFocusHandle);
	GetWorld()->GetTimerManager().ClearTimer(OnLostSightOfPawnHandle);
	Super::EndPlay(EndPlayReason);
}

void AEnemyAIController::AdjustCustomFocus()
{
	if (!Proprietor->GetIsActionInMotion())
	{
		FVector TargetLocation = bIsUsingEQS ? EnemyCommunicator->GetTargetLocation() : StateTreeData.GetMoveLocation();
		if ((Proprietor->GetMoveMode() != EMoveMode::Walk || TargetLocation.ContainsNaN()) && bIsControlRigAvailableToToggle) Proprietor->GetAnimInstance()->SetIsControlRigEnabled(false);
		else
		{
			if (bIsControlRigAvailableToToggle) Proprietor->GetAnimInstance()->SetIsControlRigEnabled(true);
			const TObjectPtr<AEnemy> Player = EnemyCommunicator->GetTarget();
			if (bPredictionEnabled && Player)
			{
				const FVector MuzzleLocation = Proprietor->GetWeapon()->GetSocketLocation("MuzzleSocket");
				const FVector ToTarget = TargetLocation - MuzzleLocation;
				const float Distance = ToTarget.Size();
				if (Distance < PredictionStartsAt)
				{
					Proprietor->GetAnimInstance()->SetLookAtLocation(TargetLocation);
					if (bCustomFocusEnabled) CustomFocusToLocation(TargetLocation);
					return;
				}
				FVector TargetVelocity = Player->GetCharacterMovement()->Velocity;
				TargetVelocity.Z = 0.f;
				const float BasePredictionTime = Distance / ProjectileSpeed * PredictionTimeMultiplier;
				float PredictionTime = bPredictionClampEnabled ? FMath::Clamp(BasePredictionTime, 0.f, PredictionMaxClamp) : BasePredictionTime;
				if (TargetLocation.Z - MuzzleLocation.Z > VerticalDeltaThreshold) TargetLocation.Z += VerticalDeltaAddition;
				if (PredictionTime > DistanceScalingMultiplierStart) PredictionTime *= DistanceScalingMultiplier;
				const FVector ModifiedTargetLocation = TargetLocation + TargetVelocity * PredictionTime;
				Proprietor->GetAnimInstance()->SetLookAtLocation(ModifiedTargetLocation);
				if (bCustomFocusEnabled) CustomFocusToLocation(ModifiedTargetLocation);
			}
			else
			{
				Proprietor->GetAnimInstance()->SetLookAtLocation(TargetLocation);
				if (bCustomFocusEnabled) CustomFocusToLocation(TargetLocation);
			}
		}
	}
}

void AEnemyAIController::SetIsTargetInMySight(const bool bState) { StateTreeData.SetIsTargetInMySight(bState); }

void AEnemyAIController::OnLostSightOfPawn() { SetIsTargetInMySight(false); }

void AEnemyAIController::StopStateTreeLogic(const bool bNeedCleanup) const
{
	if (StateTree)
	{
		StateTree->StopLogic(TEXT("Stopping State Tree"));
		if (bNeedCleanup) StateTree->Cleanup();
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		GetWorld()->GetTimerManager().ClearTimer(OnLostSightOfPawnHandle);
		if (!StateTreeData.GetIsTargetInMySight())
		{
			Proprietor->GetAnimInstance()->SetIsTargetInSight(true);
			SetIsTargetInMySight(true);
		}
	}
	else GetWorld()->GetTimerManager().SetTimer(OnLostSightOfPawnHandle, this, &AEnemyAIController::OnLostSightOfPawn, 0.3f, false);
}

void AEnemyAIController::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (!Proprietor) return;
	const FTransform Transform = Proprietor->GetMesh()->GetSocketTransform("SensorSocket", RTS_World);
	OutLocation = Transform.GetLocation();
	OutRotation = Transform.GetRotation().Rotator();
}

void AEnemyAIController::CustomFocusToLocation(const FVector& InTargetLocation) const
{
	FRotator NewControlRotation = GetControlRotation();
	NewControlRotation.Yaw = (InTargetLocation - GetPawn()->GetActorLocation()).GetSafeNormal2D().Rotation().Yaw;
	GetPawn()->SetActorRotation(NewControlRotation);
}

void AEnemyAIController::SetTarget(const TObjectPtr<AEnemy>& InTarget, const bool bIsTargetAcquiredForFirstTime)
{
	StateTreeData.SetTargetRef(InTarget);
	if (!bIsTargetAcquiredForFirstTime) SetupStateTree();
}

void AEnemyAIController::SetupEQSTarget()
{
	EQSTarget = GetWorld()->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass());
	EQSTarget->SetActorHiddenInGame(true);
	EQSTarget->SetActorEnableCollision(false);
}

void AEnemyAIController::SetEQSTargetLocation()
{
	if (!bIsUsingEQS) return;
	const FVector PointLocation = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetPointLocation(Proprietor->GetCharacterName(), Proprietor->GetActorLocation());
	(void)OnEqsTargetUpdate.ExecuteIfBound();
	if (!PointLocation.ContainsNaN()) SetTargetLocation(PointLocation);
}

void AEnemyAIController::SetTargetLocation(const FVector& InLocation) { GetStateTreeData().SetEQSPointLocation(InLocation); }

void AEnemyAIController::StopStateTreeLogic() const
{
	if (StateTree)
	{
		StateTree->StopLogic(TEXT("Stopping State Tree"));
		StateTree->Cleanup();
	}
}
