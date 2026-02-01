#include "PyroBossWeapon.h"

void APyroBossWeapon::AI_Shoot()
{
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::PyroBossProjectile, false);
}

void APyroBossWeapon::BeginPlay()
{
	Name = EWeapon::PyroBossWeapon;
	Super::BeginPlay();
}
