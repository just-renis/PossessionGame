#include "SpyWeapon.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void ASpyWeapon::P_Shoot()
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
//	PlaySound(ESound::TricksterShoot);
	MuzzleFlashLogic();
	MakeShootingCameraShake();
	CurrentAmmo--;
	ShootTimer = 0.f;
	bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(EPoolObject::SpyProjectile, false, true);
}

void ASpyWeapon::P_Reload(int32 OtherAmmoSource)
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	Super::P_Reload(OtherAmmoSource);
}

void ASpyWeapon::BeginPlay()
{
	Name = EWeapon::SpyWeapon;
	Super::BeginPlay();
}

void ASpyWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const bool bFromFirstWeapon)
{
	Super::ShootProjectile(InProjectileName, bIsFromAbility);
	if (CurrentAmmo <= MagazineCap * 0.1f) OnAmmoPercentageChanged.Broadcast(10);
	else if (CurrentAmmo <= MagazineCap * 0.5f) OnAmmoPercentageChanged.Broadcast(50);
}

void ASpyWeapon::SetConceal(bool bState)
{
	Mesh->SetScalarParameterValueOnMaterials("Opacity", bState ? 0.2f : 1.f);
	HandsMesh->SetScalarParameterValueOnMaterials("Opacity", bState ? 0.2f : 1.f);
}

void ASpyWeapon::AI_Shoot()
{
	//PlaySound(ESound::TricksterShoot);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::SpyProjectile, false, true);
}
