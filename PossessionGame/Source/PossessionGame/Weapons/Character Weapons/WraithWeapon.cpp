#include "WraithWeapon.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

AWraithWeapon::AWraithWeapon()
{
	ShootSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ShootSfx"));
	ShootSfx->SetAutoActivate(false);
	ShootSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::WraithShoot, ShootSfx);
}

void AWraithWeapon::AI_Shoot()
{
	PlaySound(ESound::WraithShoot, true);
	ShootTimer = 0.f;
	ShootProjectile(EPoolObject::AIWraithProjectile, false);
}

void AWraithWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(CurrentAmmo); }

void AWraithWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	AssetsManager->PlaySound2D(ESound::WraithShoot);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(InProjectileName);
}

void AWraithWeapon::BeginPlay()
{
	Name = EWeapon::WraithWeapon;
	Super::BeginPlay();
}
