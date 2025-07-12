#include "SpyAbility2.h"
#include "Camera/CameraComponent.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

void USpyAbility2::AI_Execute()
{
	if (CanExecute())
	{
		Proprietor->SetActorLocation(GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetFarthestEnemyLocation(Proprietor->GetActorLocation()));
		Proprietor->StopConcealingAbility(false);
		Super::AI_Execute();
	}
}

void USpyAbility2::P_Execute() { CastLineTrace(); }

bool USpyAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USpyAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ASpy>(AbilityProprietor); }

void USpyAbility2::CastLineTrace()
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	FVector StartLocation = Proprietor->GetCamera()->GetComponentLocation();
	FVector EndLocation = StartLocation + Proprietor->GetCamera()->GetForwardVector() * 15000.f;
	QueryParams.AddIgnoredActor(Proprietor);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, Proprietor->GetXRayState() ? ECC_GameTraceChannel3 : ECC_GameTraceChannel5, QueryParams)) TryHitEnemy(HitResult);
}

void USpyAbility2::TryHitEnemy(FHitResult& HitResult)
{
	if (HitResult.GetActor()->ActorHasTag("Enemy"))
	{
		TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy->GetIsAlive()) Execute(HitResult);
	}
}

void USpyAbility2::Execute(FHitResult& HitResult)
{
	Proprietor->SetActorLocation(HitResult.Location);
	Super::P_Execute();
	Cast<AEnemy>(HitResult.GetActor())->InitiateDeath();
	Proprietor->StopConcealingAbility();
}
