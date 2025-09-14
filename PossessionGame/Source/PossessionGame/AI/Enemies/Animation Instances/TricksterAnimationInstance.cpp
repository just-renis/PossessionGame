#include "TricksterAnimationInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/AI/Enemies/Controllers/TricksterAIController.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTricksterAnimationInstance::InnerPlayMontage(const EEnemyAnimState InState)
{
	Super::InnerPlayMontage(InState);
	if (InState == EEnemyAnimState::GrenadeLaunch)
	{
		GetWorld()->GetTimerManager().SetTimer(TriggerGrenadeProjectionHandle, this, &UTricksterAnimationInstance::TriggerGrenadeProjection, 0.45f, false);
		GetWorld()->GetTimerManager().SetTimer(LaunchGrenadeHandle, this, &UTricksterAnimationInstance::LaunchGrenade, 1.03f, false);
	}
}

void UTricksterAnimationInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* PoisonGrenadeLaunchMontage = Montages.Find(EEnemyAnimState::GrenadeLaunch);
	if (PoisonGrenadeLaunchMontage && Montage == PoisonGrenadeLaunchMontage->Get())
	{
		Proprietor->SetIsActionInMotion(false);
		(void)Cast<ATricksterAIController>(Proprietor->GetAI())->OnPoisonGrenadeLaunched.ExecuteIfBound();
	}
}

void UTricksterAnimationInstance::LaunchGrenade() const
{
	if (Proprietor->IsAsyncAssetStillAllowedToBeUsed()) Cast<ATrickster>(Proprietor)->AI_LaunchPoisonGrenade();
}

void UTricksterAnimationInstance::TriggerGrenadeProjection() const
{
	FProjectileData* ProjectileData = Proprietor->GetWeapon()->GetProjectileData(EPoolObject::TricksterPoisonProjectile);
	if (!ProjectileData) return;
	const int32 ProjectileAmount = 1 + GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EPoolObject::TricksterPoisonProjectile, EUpgradeAttribute::ProjectilesAmount, false, true);
	const FAdditionalProjectiles AdditionalProjectilesData = ProjectileData->GetMultiProjectileData(ProjectileAmount);
	for (int32 i = 0; i < ProjectileAmount; i++)
	{
		FPredictProjectilePathParams PathParams;
		PathParams.StartLocation = Proprietor->GetWeapon()->GetSocketLocation(ProjectileData->ProjectileSocketName);
		const FRotator BaseRotation = Proprietor->GetWeapon()->GetSocketRotation(ProjectileData->ProjectileSocketName);
		const FVector FinalDirection = BaseRotation.RotateVector(AdditionalProjectilesData.Data[i].Direction);
		PathParams.LaunchVelocity = FinalDirection.GetSafeNormal() * AdditionalProjectilesData.Data[i].MaxSpeed;
		PathParams.bTraceWithCollision = true;
		PathParams.ProjectileRadius = 1.f;
		PathParams.MaxSimTime = 8.f;
		PathParams.SimFrequency = 30.f;
		PathParams.OverrideGravityZ = GetWorld()->GetGravityZ() * AdditionalProjectilesData.Data[i].GravityScale;
		PathParams.TraceChannel = ECC_GameTraceChannel6;
		FPredictProjectilePathResult PathResult;
		if (UGameplayStatics::PredictProjectilePath(GetWorld(), PathParams, PathResult))
		{
			FRotator MarkerRotation = PathResult.HitResult.ImpactNormal.Rotation();
			MarkerRotation.Pitch -= 90.f; 
			AssetsManager->SpawnVfx(EVfx::TricksterGrenadeLauncherMarker, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, PathResult.HitResult.Location, MarkerRotation);
		}
	}
}

void UTricksterAnimationInstance::ClearTimers()
{
	GetWorld()->GetTimerManager().ClearTimer(TriggerGrenadeProjectionHandle);
	GetWorld()->GetTimerManager().ClearTimer(LaunchGrenadeHandle);
	Super::ClearTimers();
}
