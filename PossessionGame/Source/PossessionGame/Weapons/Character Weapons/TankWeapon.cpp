#include "TankWeapon.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ATankWeapon::AI_Shoot()
{
	if (!AnimInstance->IsPlaying(EWeaponAnimState::Shoot)) AnimInstance->PlayMontage(EWeaponAnimState::Shoot);
	ShootProjectile(EPoolObject::TankProjectile, false);
	SubtractShootCycleUntilReload();
	GetShootCyclesUntilReload() <= 0 ? Proprietor->AI_ReloadLogic() : Proprietor->AI_SetShootPause();
	Super::AI_Shoot();
}

void ATankWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(CurrentAmmo); }

void ATankWeapon::P_Shoot()
{
	if (AnimInstance->IsPlaying(EWeaponAnimState::Shoot))
	{
		Super::P_Shoot();
		if (CurrentAmmo <= 0) (void)OnAmmoEnded.ExecuteIfBound();
	}
}

void ATankWeapon::P_Reload(int32 OtherAmmoSource)
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	Super::P_Reload(OtherAmmoSource);
}

void ATankWeapon::P_SetupTransfer(bool bSetToFirstHand)
{
	Super::P_SetupTransfer(bSetToFirstHand);
	AnimInstance->StopAllMontages(0.f);
}

void ATankWeapon::BeginPlay()
{
	Name = EWeapon::TankWeapon;
	Super::BeginPlay();
	SetWeaponReloadMontageDelegate();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}
