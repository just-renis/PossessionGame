#include "SpyWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void ASpyWeapon::P_Shoot(bool bIsFirstWeapon)
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(bIsFirstWeapon);
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
	SetupHandsAnimInstance();
}

void ASpyWeapon::ShootProjectile(EPoolObject ProjectileName)
{
	Super::ShootProjectile(ProjectileName);
	if (CurrentAmmo <= MagazineCap * 0.1f) OnAmmoPercentageChanged.Broadcast(10);
	else if (CurrentAmmo <= MagazineCap * 0.5f) OnAmmoPercentageChanged.Broadcast(50);
}

void ASpyWeapon::SetConceal(bool bState)
{
	Mesh->SetScalarParameterValueOnMaterials("Opacity", bState ? 0.2f : 1.f);
	HandsMesh->SetScalarParameterValueOnMaterials("Opacity", bState ? 0.2f : 1.f);
}
