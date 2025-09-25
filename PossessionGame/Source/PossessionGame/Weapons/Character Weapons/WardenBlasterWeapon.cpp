#include "WardenBlasterWeapon.h"
#include "Components/AudioComponent.h"

AWardenBlasterWeapon::AWardenBlasterWeapon()
{
	ShootSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ShootSound"));
	ShootSfx->SetAutoActivate(false);
	ShootSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::WardenShoot, ShootSfx);
}

void AWardenBlasterWeapon::AI_Shoot()
{
	PlaySound(ESound::WardenShoot, true);
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
