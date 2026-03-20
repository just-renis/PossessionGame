#include "SamuraiWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ASamuraiWeapon::AI_Shoot()
{
	HandleMeleeStrike();
	ShootTimer = 0.f;
}

void ASamuraiWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	//PlayHandsAndWeaponMontage(EHandsAnimState::Shoot, EWeaponAnimState::Shoot);
	Super::P_Shoot(InProjectileName);
}

void ASamuraiWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(CurrentAmmo); }

void ASamuraiWeapon::BeginPlay()
{
	Name = EWeapon::SamuraiWeapon;
	Super::BeginPlay();
//	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
//	AnimInstance->SetWeaponName(Name);
}

void ASamuraiWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, const bool bFromFirstWeapon)
{
	
	Super::ShootProjectile(EPoolObject::SamuraiProjectile, false);
}

void ASamuraiWeapon::HandleMeleeStrike()
{
	/*FCollisionQueryParams OverlapParams;
	OverlapParams.AddIgnoredActor(this);
	TArray<FOverlapResult> Overlaps;
	const FVector Origin = GetWeapon()->GetHandsSocketLocation("PlayerWeaponSocket1") - GetActorForwardVector() * 30.f;
	GetWorld()->OverlapMultiByProfile(Overlaps, Origin, FQuat::Identity, FName("TriggerDamageable"), FCollisionShape::MakeSphere(QuickMeleeAttackRadiusInMeters * 100.f), OverlapParams);
	for (const FOverlapResult& Overlap : Overlaps)
	{
		TObjectPtr<AActor> Actor = Overlap.GetActor();
		if (!Actor || (!Actor->ActorHasTag("AliveEnemy") && !Actor->ActorHasTag("Destructible"))) return;
		FVector ToTarget = (Actor->GetActorLocation() - Origin).GetSafeNormal();
		if (FVector::DotProduct(GetActorForwardVector(), ToTarget) < 0.342f) return;
		FHitResult Hit;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		TraceParams.AddIgnoredActor(Actor);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, Actor->GetActorLocation(), ECC_Visibility, TraceParams)) return;
		UGameplayStatics::ApplyDamage(Actor, P_GetMeleeDamage(), GetController(), this, UDamageType::StaticClass());
		(void)DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(false);
	}*/
}
