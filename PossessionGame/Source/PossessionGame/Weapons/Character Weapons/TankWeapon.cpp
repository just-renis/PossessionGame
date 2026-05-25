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
	if (!HandBallVfx) AssetsManager->AttachVfx(EVfx::TankHandBall, [this]() -> bool { return Proprietor ? Proprietor->GetIsAlive() : false; }, &HandBallVfx, HandsMesh, FName("BallSocket"), Proprietor->GetVariation());
	else AssetsManager->ActivateWithVariation(EVfx::TankHandBall, nullptr, HandBallVfx, false, Proprietor->GetVariation());
}

void ATankWeapon::P_BallRelease()
{
	if (!HandBallVfx) return;
	HandBallVfx->SetVariableFloat(FName("1"), 0.f);
	HandBallVfx->SetVariableFloat(FName("2"), 0.f);
	HandBallVfx->SetVariableFloat(FName("3"), 0.f);
	HandBallVfx->SetVariableFloat(FName("4"), 0.f);
	HandBallVfx->SetVariableFloat(FName("5"), 0.f);
	HandBallVfx->DeactivateImmediate();
	AssetsManager->ActivateWithVariation(EVfx::TankHandBall, nullptr, HandBallVfx, false, Proprietor->GetVariation());
	GetWorldTimerManager().SetTimer(UpdateHandBallVfxHandle, this, &ATankWeapon::P_UpdateHandBallVfx, 0.025f, true);
}

void ATankWeapon::P_Setup()
{
	Super::P_Setup();
	Mesh->SetComponentTickEnabled(true);
}

void ATankWeapon::P_UpdateHandBallVfx()
{
	if (!HandBallVfx) return;
	UpdateHandBallLoopCount++;
	HandBallVfx->SetVariableFloat(FName("1"), 0.0045f * UpdateHandBallLoopCount);
	HandBallVfx->SetVariableFloat(FName("2"), 0.1163f * UpdateHandBallLoopCount);
	HandBallVfx->SetVariableFloat(FName("3"), 0.4678f * UpdateHandBallLoopCount);
	HandBallVfx->SetVariableFloat(FName("4"), 0.6463f * UpdateHandBallLoopCount);
	HandBallVfx->SetVariableFloat(FName("5"), 0.6131f * UpdateHandBallLoopCount);
	AssetsManager->ActivateWithVariation(EVfx::TankHandBall, nullptr, HandBallVfx, false, Proprietor->GetVariation());
	if (UpdateHandBallLoopCount >= 10)
	{
		UpdateHandBallLoopCount = 0;
		GetWorldTimerManager().ClearTimer(UpdateHandBallVfxHandle);
	}
}

void ATankWeapon::BeginPlay()
{
	Name = EWeapon::TankWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UTankWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}
