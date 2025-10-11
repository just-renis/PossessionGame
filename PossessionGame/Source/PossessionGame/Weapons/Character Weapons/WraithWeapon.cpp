#include "WraithWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void AWraithWeapon::P_UpdateAmmoTextHUD()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(-1);
}

void AWraithWeapon::BeginPlay()
{
	Name = EWeapon::WraithWeapon;
	Super::BeginPlay();
}
