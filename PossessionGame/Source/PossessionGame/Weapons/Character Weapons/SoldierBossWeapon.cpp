#include "SoldierBossWeapon.h"
#include "Components/AudioComponent.h"

ASoldierBossWeapon::ASoldierBossWeapon()
{
	ShootSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ShootSfx"));
	ShootSfx->SetAutoActivate(false);
	ShootSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::SoldierBossShoot, ShootSfx);
}

void ASoldierBossWeapon::AI_Shoot()
{
	PlaySound(ESound::SoldierBossShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::SoldierBossProjectile, false);
}

void ASoldierBossWeapon::BeginPlay()
{
	Name = EWeapon::SoldierBossWeapon;
	Super::BeginPlay();
}
