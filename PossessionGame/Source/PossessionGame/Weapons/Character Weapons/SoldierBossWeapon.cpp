#include "SoldierBossWeapon.h"

void ASoldierBossWeapon::AI_Shoot()
{
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::SoldierBossProjectile, false);
}

void ASoldierBossWeapon::BeginPlay()
{
	Name = EWeapon::SoldierBossWeapon;
	Super::BeginPlay();
}
