#include "PyroBossWeapon.h"
#include "Components/AudioComponent.h"

APyroBossWeapon::APyroBossWeapon()
{
	ShootSfx = CreateDefaultSubobject<UAudioComponent>("ShootSfx");
	ShootSfx->SetAutoActivate(false);
	ShootSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::PyroBossShoot, ShootSfx);
}

void APyroBossWeapon::AI_Shoot()
{
	PlaySound(ESound::PyroBossShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::PyroBossProjectile, false);
}

void APyroBossWeapon::BeginPlay()
{
	Name = EWeapon::PyroBossWeapon;
	Super::BeginPlay();
}
