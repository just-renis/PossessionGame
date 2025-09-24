#include "WardenBlasterWeapon.h"

void AWardenBlasterWeapon::AI_Shoot()
{
	Super::AI_Shoot();
	if (bFirstWeaponTrigger) ShootProjectile(EPoolObject::WardenProjectile, false);
	else ShootProjectile(EPoolObject::WardenProjectile, false, "SecondProjectileSocket");
	bFirstWeaponTrigger = !bFirstWeaponTrigger;
}

void AWardenBlasterWeapon::BeginPlay()
{
	Name = EWeapon::WardenBlasterWeapon;
	Super::BeginPlay();
}
