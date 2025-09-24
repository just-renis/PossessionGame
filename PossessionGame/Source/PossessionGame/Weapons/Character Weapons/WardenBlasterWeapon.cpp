#include "WardenBlasterWeapon.h"

void AWardenBlasterWeapon::AI_Shoot()
{
	Super::AI_Shoot();UE_LOG(LogTemp, Error, TEXT("AI_Shoot ShootProjectile"));
	ShootProjectile(EPoolObject::WardenProjectile, false);
}

void AWardenBlasterWeapon::BeginPlay()
{
	Name = EWeapon::WardenBlasterWeapon;
	Super::BeginPlay();
}
