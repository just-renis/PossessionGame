#include "WraithWeapon.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void AWraithWeapon::P_UpdateAmmoTextHUD()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(-1);
}

void AWraithWeapon::P_Shoot()
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	MuzzleFlashLogic();
	MakeShootingCameraShake();
	ShootTimer = 0.f;
	bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(EPoolObject::WraithProjectile, false);
}

void AWraithWeapon::BeginPlay()
{
	Name = EWeapon::WraithWeapon;
	Super::BeginPlay();
}
