#include "EnemyAIController.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAnimationInstance.h"
#include "Components/StateTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/EmptyActor.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

AEnemyAIController::AEnemyAIController()
{
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("perceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("sightConfig"));
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2000.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	Perception->ConfigureSense(*SightConfig);
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	StateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTree->SetStartLogicAutomatically(false);
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
	UKismetSystemLibrary::K2_ClearTimer(this, "SightOnTargetLogic");
	UKismetSystemLibrary::K2_ClearTimer(this, "OnLostSightOfPawn");
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
	UKismetSystemLibrary::K2_SetTimer(this, "AdjustCustomFocus", 0.01f, true);
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
	UKismetSystemLibrary::K2_SetTimer(this, "SightOnTargetLogic", SightOnTargetRepeat, true);
}

void AEnemyAIController::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (!Proprietor) return;
	const FTransform Transform = Proprietor->GetMesh()->GetSocketTransform("SensorSocket", RTS_World);
	OutLocation = Transform.GetLocation();
	OutRotation = Transform.GetRotation().Rotator();
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		UKismetSystemLibrary::K2_ClearTimer(this, "OnLostSightOfPawn");
		if (!StateTreeData.GetIsTargetInMySight())
		{
			Proprietor->GetAnimInstance()->SetIsTargetInSight(true);
			SetIsTargetInMySight(true);
		}
	}
	else UKismetSystemLibrary::K2_SetTimer(this, "OnLostSightOfPawn", 0.3f, false);
}

void AEnemyAIController::AdjustCustomFocus() const
{
	if (!Proprietor->GetIsActionInMotion() && EnemyCommunicator->GetTarget()) CustomFocusToLocation(EnemyCommunicator->GetTargetLocation());
}

void AEnemyAIController::CustomFocusToLocation(const FVector& InTargetLocation) const
{
	FRotator NewControlRotation = GetControlRotation();
	NewControlRotation.Yaw = (InTargetLocation - GetPawn()->GetActorLocation()).GetSafeNormal2D().Rotation().Yaw;
	GetPawn()->SetActorRotation(NewControlRotation);
}

void AEnemyAIController::SetTarget(const TObjectPtr<AActor>& InTarget, const bool bIsTargetAcquiredForFirstTime)
{
	StateTreeData.SetTargetRef(InTarget);
	if (!bIsTargetAcquiredForFirstTime) SetupStateTree();
}

void AEnemyAIController::SetIsTargetInMySight(const bool bState) { StateTreeData.SetIsTargetInMySight(bState); }

void AEnemyAIController::SetupEQSTarget()
{
	EQSTarget = GetWorld()->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass());
	EQSTarget->SetActorHiddenInGame(true);
	EQSTarget->SetActorEnableCollision(false);
}

void AEnemyAIController::SetEQSTargetLocation()
{
	const FVector PointLocation = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetPointLocation(Proprietor->GetCharacterName(), Proprietor->GetActorLocation());
	if (!PointLocation.ContainsNaN()) GetStateTreeData().SetEQSPointLocation(PointLocation);
}

void AEnemyAIController::StopStateTreeLogic() const
{
	if (StateTree)
	{
		StateTree->StopLogic(TEXT("Stopping State Tree"));
		StateTree->Cleanup();
	}
}

void AEnemyAIController::StopStateTreeLogic(const bool bNeedCleanup) const
{
	if (StateTree)
	{
		StateTree->StopLogic(TEXT("Stopping State Tree"));
		if (bNeedCleanup) StateTree->Cleanup();
	}
}


void AEnemyAIController::OnLostSightOfPawn() { SetIsTargetInMySight(false); }

void AEnemyAIController::SightOnTargetLogic() const
{
	if (!Proprietor->GetIsActionInMotion()) Proprietor->GetAnimInstance()->SetLookAtLocation(EnemyCommunicator->GetTargetLocation());
}
