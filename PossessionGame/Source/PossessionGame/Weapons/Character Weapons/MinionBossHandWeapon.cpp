#include "MinionBossHandWeapon.h"

void AMinionBossHandWeapon::AI_Shoot()
{
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::MinionBossProjectile);
}

void AMinionBossHandWeapon::BeginPlay()
{
	Name = EWeapon::MinionBossHandWeapon;
	Super::BeginPlay();
}
