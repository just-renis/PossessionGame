#include "EnemyAIController.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyAnimationInstance.h"
#include "Components/StateTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PossessionGame/Utilities/Global Utilities/EmptyActor.h"
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

void AEnemyAIController::SetShootCooldown(float Value) { StateTreeData.SetShootCooldown(Value); }

void AEnemyAIController::SetReloadDelay(float Value) { StateTreeData.SetReloadDelay(Value); }

void AEnemyAIController::DestroyThis()
{
	if (Perception)
	{
		Perception->Deactivate();
		Perception->OnPerceptionUpdated.Clear();
	}
	EnemyCommunicator->GetEQSDelegate().RemoveAll(this);
	UKismetSystemLibrary::K2_ClearTimer(this, "SightOnTargetLogic");
	UKismetSystemLibrary::K2_ClearTimer(this, "OnLostSightOfPawn");
	StateTree->Cleanup();
}

void AEnemyAIController::SetProprietor(TObjectPtr<AEnemy> Enemy) { Proprietor = Enemy; }

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

void AEnemyAIController::SetMoveMode(EMoveMode MoveMode) { Proprietor->AI_SetMoveMode(MoveMode); }

void AEnemyAIController::SetupAI()
{
	UKismetSystemLibrary::K2_SetTimer(this, "AdjustCustomFocus", 0.02f, true);
	EnemyCommunicator->GetEQSDelegate().AddUObject(this, &AEnemyAIController::SetEQSTargetLocation);
	if (EnemyCommunicator->GetTarget()) SetTarget(EnemyCommunicator->GetTarget(), false);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTargetChange.AddUObject(this, &AEnemyAIController::SetTarget);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	EnemyCommunicator = GetWorld()->GetSubsystem<UEnemyCommunicator>();
	SetupEQSTarget();
}

void AEnemyAIController::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (!Proprietor) return;
	FTransform Transform = Proprietor->GetMesh()->GetSocketTransform("SensorSocket", RTS_World);
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
			DrawDebugLine(GetWorld(), GetPawn()->GetPawnViewLocation(), Actor->GetActorLocation(), FColor::Red, false, 2.f);
		}
	}
	else UKismetSystemLibrary::K2_SetTimer(this, "OnLostSightOfPawn", 0.3f, false);
}

void AEnemyAIController::CustomFocusToLocation(FVector TargetLocation)
{
	FRotator NewControlRotation = GetControlRotation();
	NewControlRotation.Yaw = (TargetLocation - GetPawn()->GetActorLocation()).GetSafeNormal2D().Rotation().Yaw;
	GetPawn()->SetActorRotation(NewControlRotation);
}

void AEnemyAIController::SetTarget(TObjectPtr<AActor> Target, bool bWasTargetSet)
{
	StateTreeData.SetTargetRef(Target);
	if (!bWasTargetSet) SetupStateTree();
}

void AEnemyAIController::SetIsTargetInMySight(bool bState)
{
	StateTreeData.SetIsTargetInMySight(bState);
	if (bState) UKismetSystemLibrary::K2_SetTimer(this, "SightOnTargetLogic", SightOnTargetRepeat, true);
	else UKismetSystemLibrary::K2_ClearTimer(this, "SightOnTargetLogic");
}

void AEnemyAIController::SetupEQSTarget()
{
	EQSTarget = GetWorld()->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass());
	EQSTarget->SetActorHiddenInGame(true);
	EQSTarget->SetActorEnableCollision(false);
}

void AEnemyAIController::SetEQSTargetLocation()
{
	FVector PointLocation = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetPointLocation(Proprietor->GetCharacterName(), Proprietor->GetActorLocation());
	if (!PointLocation.ContainsNaN()) GetStateTreeData().SetEQSPointLocation(PointLocation);
}

void AEnemyAIController::OnLostSightOfPawn() { SetIsTargetInMySight(false); }

void AEnemyAIController::SightOnTargetLogic()
{
	const float CurrentDistanceBetweenPlayerZ = FMath::Abs(EnemyCommunicator->GetTargetLocation().Z - GetPawn()->GetActorLocation().Z);
	if (!FMath::IsNearlyEqual(DistanceBetweenTargetZ, CurrentDistanceBetweenPlayerZ, 0.01f))
	{
		DistanceBetweenTargetZ = CurrentDistanceBetweenPlayerZ;
		Proprietor->GetAnimInstance()->SetAimPitch(UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), EnemyCommunicator->GetTargetLocation()).Pitch);
	}
}

void AEnemyAIController::AdjustCustomFocus() { if (EnemyCommunicator->GetTarget()) CustomFocusToLocation(EnemyCommunicator->GetTargetLocation()); }
