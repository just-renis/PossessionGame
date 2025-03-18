#include "RangerAnimationInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/AI/Enemies/Controllers/RangerAIController.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_VfxContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAnimationInstance::InnerPlayMontage(EEnemyAnimState State)
{
	Super::InnerPlayMontage(State);
//	UE_LOG(LogTemp, Warning, TEXT("ANIMATION PLAYED: %s"), *UEnum::GetValueAsString(State));
	if (State == EEnemyAnimState::GrenadeLaunch)
	{
		UKismetSystemLibrary::K2_SetTimer(this, "TriggerGrenadeProjection", 0.1f, false);
		UKismetSystemLibrary::K2_SetTimer(this, "LaunchGrenade", 1.1f, false);
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
	if (DeployShieldMontage && Montage == DeployShieldMontage->Get()) Proprietor->SetActionInMotion(false);
}

void URangerAnimationInstance::SpawnOrLoadGrenadeLaunchMarker(const EVfx InVfxName, const FVector& InLandingPosition, const FRotator& InLandingRotation) const
{
	if (AssetsManager->GetVfxData(InVfxName)->GetVfxEffect().IsValid()) SpawnGrenadeLaunchMarker(InVfxName, InLandingPosition, InLandingRotation);
	else AssetsManager->GetVfxData(InVfxName)->LoadVfx(GetWorld(), [this, InVfxName, InLandingPosition, InLandingRotation] { SpawnGrenadeLaunchMarker(InVfxName, InLandingPosition, InLandingRotation); });
}

void URangerAnimationInstance::SpawnGrenadeLaunchMarker(const EVfx InVfxName, const FVector& InLandingPosition, const FRotator& InLandingRotation) const
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AssetsManager->GetVfxData(InVfxName)->GetVfxEffect().Get(), InLandingPosition, InLandingRotation);
}

void URangerAnimationInstance::LaunchGrenade() const { if (Proprietor->GetIsAlive()) Cast<ARanger>(Proprietor)->LaunchGrenade(); }

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
		SpawnOrLoadGrenadeLaunchMarker(EVfx::RangerGrenadeLauncherMarker, PathResult.HitResult.Location, MarkerRotation);
	}
}
