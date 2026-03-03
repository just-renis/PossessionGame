#include "TankWeapon.h"

#include "NiagaraComponent.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Animation Instances/Weapons/TankWeaponAnimInstance.h"
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

void ATankWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	if (AnimInstance->IsPlaying(EWeaponAnimState::Shoot))
	{
		AssetsManager->PlaySound2D(ESound::TankShoot);
		Super::P_Shoot(InProjectileName);
		if (CurrentAmmo <= 0) (void)OnAmmoEnded.ExecuteIfBound();
	}
}

void ATankWeapon::P_Reload() { HandsAnimInstance->PlayMontage(EHandsAnimState::Reload); }

void ATankWeapon::P_SetupTransfer(bool bSetToFirstHand)
{
	Super::P_SetupTransfer(bSetToFirstHand);
	AnimInstance->StopAllMontages(0.f);
	if (!HandsMesh) return;
	if (!HandBallVfx) AssetsManager->AttachVfx(EVfx::TankHandBall, [this]() -> bool { return Proprietor ? Proprietor->GetIsAlive() : false; }, &HandBallVfx, HandsMesh, FName("BallSocket"));
	else HandBallVfx->Activate(true);
}

void ATankWeapon::P_Setup()
{
	Super::P_Setup();
	Mesh->SetComponentTickEnabled(true);
}

void ATankWeapon::BeginPlay()
{
	Name = EWeapon::TankWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UTankWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}
