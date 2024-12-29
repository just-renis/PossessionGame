#include "RangerAbility1.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAbility1::AI_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::GrenadeLaunch);
	Proprietor->PlaySound(ESound::RangerGrenadeLaunchAim);
	FPredictProjectilePathParams PathParams;
	PathParams.StartLocation = Proprietor->GetWeapon()->GetSocketLocation(FName("GrenadeLauncherSocket"));
	PathParams.LaunchVelocity = FMath::VRandCone(Proprietor->GetWeapon()->GetSocketRotation(FName("GrenadeLauncherSocket")).Vector(), FMath::DegreesToRadians(0)) * 300.f;
	PathParams.bTraceWithCollision = true;
	PathParams.ProjectileRadius = 1.f;
	PathParams.MaxSimTime = 8.f;
	PathParams.SimFrequency = 30.f;
	PathParams.OverrideGravityZ = GetWorld()->GetGravityZ() * 0.8f;
	PathParams.TraceChannel = ECC_GameTraceChannel6;
	FPredictProjectilePathResult PathResult;
	if (UGameplayStatics::PredictProjectilePath(GetWorld(), PathParams, PathResult))
	{
		FVector LandingPosition = PathResult.HitResult.Location;
		DrawDebugSphere(GetWorld(), LandingPosition, 50.f, 12, FColor::Red, false, 10.f);
	}
	Super::AI_Execute();
}

void URangerAbility1::P_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::GrenadeLaunch);
	Proprietor->LaunchGrenade();
	Super::P_Execute();
}

bool URangerAbility1::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void URangerAbility1::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ARanger>(AbilityProprietor); }

void URangerAbility1::CooldownLogic(const float DeltaTime)
{
	Super::CooldownLogic(DeltaTime);
	if (CurrentCooldownSecs <= 0.f) Proprietor->AI_ToggleGrenadeLaunchTrigger(true);
}
