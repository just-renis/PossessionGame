#include "RangerAnimationInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/AI/Enemies/Controllers/RangerAIController.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAnimationInstance::InnerPlayMontage(const EEnemyAnimState InState)
{
	Super::InnerPlayMontage(InState);
	if (InState == EEnemyAnimState::GrenadeLaunch)
	{
		GetWorld()->GetTimerManager().SetTimer(TriggerGrenadeProjectionHandle, this, &URangerAnimationInstance::TriggerGrenadeProjection, 0.1f, false);
		GetWorld()->GetTimerManager().SetTimer(LaunchGrenadeHandle, this, &URangerAnimationInstance::LaunchGrenade, 1.1f, false);
	}
}

void URangerAnimationInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* GrenadeLaunchMontage = Montages.Find(EEnemyAnimState::GrenadeLaunch);
	if (GrenadeLaunchMontage && Montage == GrenadeLaunchMontage->Get())
	{
		Proprietor->SetActionInMotion(false);
		(void)Cast<ARangerAIController>(Proprietor->GetAI())->OnGrenadeLaunchFinished.ExecuteIfBound();
	}
	const TObjectPtr<UAnimMontage>* DeployShieldMontage = Montages.Find(EEnemyAnimState::DeployShield);
	if (DeployShieldMontage && Montage == DeployShieldMontage->Get())
	{
		Proprietor->SetActionInMotion(false);
		(void)Cast<ARangerAIController>(Proprietor->GetAI())->OnDeployShieldFinished.ExecuteIfBound();
	}
}

void URangerAnimationInstance::LaunchGrenade() const { if (Proprietor->IsAsyncAssetStillAllowedToBeUsed()) Cast<ARanger>(Proprietor)->AI_LaunchGrenade(); }

void URangerAnimationInstance::TriggerGrenadeProjection() const
{
	FPredictProjectilePathParams PathParams;
	FProjectileData ProjectileData = Proprietor->GetWeapon()->GetProjectileData(EPoolObject::RangerGrenadeLauncherProjectile);
	PathParams.StartLocation = Proprietor->GetWeapon()->GetSocketLocation(ProjectileData.ProjectileSocketName);
	PathParams.LaunchVelocity = Proprietor->GetWeapon()->GetSocketRotation(ProjectileData.ProjectileSocketName).Vector() * ProjectileData.GetMultiProjectileData(1).Data[0].MaxSpeed;
	PathParams.bTraceWithCollision = true;
	PathParams.ProjectileRadius = 1.f;
	PathParams.MaxSimTime = 8.f;
	PathParams.SimFrequency = 30.f;
	PathParams.OverrideGravityZ = GetWorld()->GetGravityZ() * ProjectileData.GetMultiProjectileData(1).Data[0].GravityScale;
	PathParams.TraceChannel = ECC_GameTraceChannel6;
	FPredictProjectilePathResult PathResult;
	if (UGameplayStatics::PredictProjectilePath(GetWorld(), PathParams, PathResult))
	{
		FRotator MarkerRotation = PathResult.HitResult.ImpactNormal.Rotation();
		MarkerRotation.Pitch -= 90.f; 
		AssetsManager->SpawnVfx(EVfx::RangerGrenadeLauncherMarker, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, PathResult.HitResult.Location, MarkerRotation);
	}
}

void URangerAnimationInstance::ClearTimers()
{
	GetWorld()->GetTimerManager().ClearTimer(TriggerGrenadeProjectionHandle);
	GetWorld()->GetTimerManager().ClearTimer(LaunchGrenadeHandle);
	Super::ClearTimers();
}
