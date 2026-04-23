#include "SamuraiWeapon.h"
#include "NiagaraComponent.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void ASamuraiWeapon::AI_Shoot()
{
	bIsFirstCheck = true; 
	GetWorldTimerManager().SetTimer(ProcessMeleeAttackHandle, this, &ASamuraiWeapon::AI_ProcessMeleeAttack, 0.01f, true);
}

void ASamuraiWeapon::AI_StopProcessingMeleeAttack() { GetWorldTimerManager().ClearTimer(ProcessMeleeAttackHandle); }

void ASamuraiWeapon::P_TriggerSlashVfx()
{
	if (!SlashVfx) AssetsManager->AttachVfx(EVfx::PlayerSamuraiSlash, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &SlashVfx, GetHandsMesh(), FName("RootSocket"));
	else SlashVfx->Activate(true);
}

void ASamuraiWeapon::AI_ProcessMeleeAttack()
{
	const FVector CurrentSocketLocation = GetMesh()->GetSocketLocation(FName("DamageSocket"));
	const FQuat CurrentSocketRotation = GetMesh()->GetSocketQuaternion(FName("DamageSocket"));
	if (bIsFirstCheck) // To return early since it would be sweep between same vector, AKA useless check.
	{
		LastSocketLocation = CurrentSocketLocation;
		bIsFirstCheck = false;
		return;
	}
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Proprietor);
	TArray<FHitResult> HitResults;
	const FCollisionShape SweepShape = FCollisionShape::MakeCapsule(AttackWidth, AttackHeight);
	if (GetWorld()->SweepMultiByChannel(HitResults, LastSocketLocation, CurrentSocketLocation, CurrentSocketRotation, ECC_GameTraceChannel9, SweepShape, Params))
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				MeleeHit(HitActor);
				AI_StopProcessingMeleeAttack();
				bIsFirstCheck = true;
				return;
			}
		}
	}
	LastSocketLocation = CurrentSocketLocation;
}

void ASamuraiWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	if (GetHandsAnimInstance()->IsPlaying(EHandsAnimState::QuickMelee)) return;
	GetHandsAnimInstance()->PlayMontage(EHandsAnimState::QuickMelee);
	P_HandleMeleeStrike();
}

void ASamuraiWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(CurrentAmmo); }

void ASamuraiWeapon::P_HandleMeleeStrike()
{
	(void)DelegateManager->MeleeAttackOccured.ExecuteIfBound();
	FCollisionQueryParams OverlapParams;
	OverlapParams.AddIgnoredActor(Proprietor);
	TArray<FOverlapResult> Overlaps;
	const FVector Origin = Proprietor->GetActorLocation() - Proprietor->GetActorForwardVector() * 30.f;
	GetWorld()->OverlapMultiByProfile(Overlaps, Origin, FQuat::Identity, FName("TriggerDamageable"), FCollisionShape::MakeSphere(P_MeleeAttackRadiusInMeters * 100.f), OverlapParams);
	for (const FOverlapResult& Overlap : Overlaps)
	{
		TObjectPtr<AActor> Actor = Overlap.GetActor();
		if (!Actor || (!Actor->ActorHasTag("AliveEnemy") && !Actor->ActorHasTag("Destructible"))) return;
		FVector ToTarget = (Actor->GetActorLocation() - Origin).GetSafeNormal();
		if (FVector::DotProduct(Proprietor->GetActorForwardVector(), ToTarget) < 0.342f) return;
		FHitResult Hit;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(Proprietor);
		TraceParams.AddIgnoredActor(Actor);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, Actor->GetActorLocation(), ECC_Visibility, TraceParams)) return;
		const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
		float P_Damage = P_DefaultDamage + RunSaveSlot->GetUpgradeAttributeValues(EPoolObject::SamuraiSlashAttack, EUpgradeAttribute::FlatDamage, true);
		const float DamagePercentage = RunSaveSlot->GetUpgradeAttributeValues(EPoolObject::SamuraiSlashAttack, EUpgradeAttribute::DamagePercentage, true);
		P_Damage += P_DefaultDamage * DamagePercentage * 0.01f;
		UGameplayStatics::ApplyDamage(Actor, P_Damage, Proprietor->GetController(), this, UDamageType::StaticClass());
	}
}

void ASamuraiWeapon::BeginPlay()
{
	Name = EWeapon::SamuraiWeapon;
	Super::BeginPlay();
}

void ASamuraiWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, const bool bFromFirstWeapon)
{
	
	Super::ShootProjectile(EPoolObject::SamuraiSlashAttack, false);
}

void ASamuraiWeapon::MeleeHit(AActor* InActor) const
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	float AI_Damage = AI_DefaultDamage + RunSaveSlot->GetUpgradeAttributeValues(EPoolObject::SamuraiSlashAttack, EUpgradeAttribute::FlatDamage, false);
	const float DamagePercentage = RunSaveSlot->GetUpgradeAttributeValues(EPoolObject::SamuraiSlashAttack, EUpgradeAttribute::DamagePercentage, false);
	AI_Damage += AI_DefaultDamage * DamagePercentage * 0.01f;
	if (InActor) UGameplayStatics::ApplyDamage(InActor, AI_Damage, Proprietor->GetInstigatorController(), Proprietor, UDamageType::StaticClass());
}
