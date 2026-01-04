#include "WraithWeapon.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void AWraithWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(CurrentAmmo); }

void AWraithWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(InProjectileName);
}

void AWraithWeapon::BeginPlay()
{
	Name = EWeapon::WraithWeapon;
	Super::BeginPlay();
}
