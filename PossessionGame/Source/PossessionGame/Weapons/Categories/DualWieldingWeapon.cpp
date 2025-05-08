#include "DualWieldingWeapon.h"
#include "OneHandedWeapon.h"
#include "PossessionGame/Data Assets/AI/Base Data/AIWeaponData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerWeaponData.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

ADualWieldingWeapon::ADualWieldingWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
}

void ADualWieldingWeapon::AI_Setup()
{
	Super::AI_Setup();
	CyclingState.bIsCycling = AI_Data->GetIsCyclingEnabled();
}

void ADualWieldingWeapon::AI_Shoot()
{
	CyclingState.bIsCycling ? CyclingModeLogic() : NonCyclingModeLogic();
}

bool ADualWieldingWeapon::P_CanShoot(bool bIsFirstWeapon)
{
	return !HandsAnimInstance->GetIsReloading() && (bIsFirstWeapon ? FirstWeapon->GetCurrentAmmoAmount() > 0 : SecondWeapon->GetCurrentAmmoAmount() > 0);
}

bool ADualWieldingWeapon::P_CanReload(int32 OtherAmmoSource) { return TotalAmmo > 0 && !HandsAnimInstance->GetIsReloading(); }

void ADualWieldingWeapon::P_Shoot(bool bIsFirstWeapon)
{
	bIsFirstWeapon ? FirstWeapon->P_Shoot() : SecondWeapon->P_Shoot();
	if (ShootTimer >= ShootCooldown)
	{
		PlayShootMontage();
		ShootTimer = 0.f;
		DelegateManager->OnWeaponAmmoChangeThreeParams.Broadcast(FirstWeapon->GetCurrentAmmoAmount(), SecondWeapon->GetCurrentAmmoAmount(), TotalAmmo);
	}
}

void ADualWieldingWeapon::P_Reload(int32 OtherAmmoSource)
{
	int32 FirstWeaponAmmoNeeded = FirstWeapon->GetAmmoNeeded();
	int32 SecondWeaponAmmoNeeded = SecondWeapon->GetAmmoNeeded();
	if (FirstWeapon->P_CanReload(TotalAmmo)) FirstWeapon->P_Reload(TotalAmmo);
	TotalAmmo -= FirstWeaponAmmoNeeded;
	if (SecondWeapon->P_CanReload(TotalAmmo)) SecondWeapon->P_Reload(TotalAmmo);
	TotalAmmo -= SecondWeaponAmmoNeeded;
	if (TotalAmmo < 0) TotalAmmo = 0;
}

void ADualWieldingWeapon::P_SetupTransfer(bool bSetToFirstHand)
{
	Super::P_SetupTransfer(true);
	FirstWeapon->P_InitialSetupTransfer();
	FirstWeapon->P_AttachWeaponToHands(HandsMesh, "PlayerWeaponSocket1");
	SecondWeapon->P_InitialSetupTransfer();
	SecondWeapon->P_AttachWeaponToHands(HandsMesh, "PlayerWeaponSocket2");
}

void ADualWieldingWeapon::P_UpdateAmmoTextHUD()
{
	if (!HandsAnimInstance->GetIsReloading()) DelegateManager->OnWeaponAmmoChangeThreeParams.Broadcast(FirstWeapon->GetCurrentAmmoAmount(), SecondWeapon->GetCurrentAmmoAmount(), TotalAmmo);
}

void ADualWieldingWeapon::P_SetShootMontage(const bool bFirstWeapon, const bool bState)
{
	if (bFirstWeapon) ShootMontageHandler.bIsFirstWeaponShooting = bState;
	else ShootMontageHandler.bIsSecondWeaponShooting = bState;
}

void ADualWieldingWeapon::SetWeaponMeshScaleToOriginal()
{
	FirstWeapon->SetWeaponMeshScaleToOriginal();
	SecondWeapon->SetWeaponMeshScaleToOriginal();
}

void ADualWieldingWeapon::DestroyThis()
{
	FirstWeapon->Destroy();
	SecondWeapon->Destroy();
	Destroy();
}

void ADualWieldingWeapon::SetToOriginalSettings()
{
	SetWeaponMeshScaleToOriginal();
	FirstWeapon->SetCastShadow(true);
	SecondWeapon->SetCastShadow(true);
}

void ADualWieldingWeapon::DeactivateWeapon()
{
	bIsPossessed = false;
	DisableHandsMesh();
	FirstWeapon->DeactivateWeapon();
	SecondWeapon->DeactivateWeapon();
}

void ADualWieldingWeapon::SetProprietor(TObjectPtr<AEnemy> InProprietor)
{
	Proprietor = InProprietor;
	FirstWeapon->SetProprietor(InProprietor);
	SecondWeapon->SetProprietor(InProprietor);
}

void ADualWieldingWeapon::PlayShootMontage() { HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot); }

void ADualWieldingWeapon::PostProprietorSetup()
{
	SetupHandsAnimInstance();
	AssetsManager->AttachOrLoadVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle), &FirstWeapon->GetMuzzleFlashComponent(), FirstWeapon->GetMesh(), FName("MuzzleSocket"), true);
	AssetsManager->AttachOrLoadVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle), &SecondWeapon->GetMuzzleFlashComponent(), SecondWeapon->GetMesh(), FName("MuzzleSocket"), true);
}

void ADualWieldingWeapon::P_SetVariationMaterial(const EVariation InVariationName)
{
	if (!P_Data) return;
	UE_LOG(LogTemp, Warning, TEXT("MATERIAL NAME: %s"), *P_Data->GetVariationMaterial(InVariationName)->GetName());
	FirstWeapon->GetMesh()->SetMaterial(0, P_Data->GetVariationMaterial(InVariationName));
	SecondWeapon->GetMesh()->SetMaterial(0, P_Data->GetVariationMaterial(InVariationName));
}

void ADualWieldingWeapon::CyclingModeLogic()
{
	CurrentAmmo--;
	ShootTimer = 0.f;
	CyclingState.bIsFirstWeaponMode ? FirstWeapon->AI_Shoot() : SecondWeapon->AI_Shoot();
	CyclingState.bIsFirstWeaponMode = !CyclingState.bIsFirstWeaponMode;
}

void ADualWieldingWeapon::NonCyclingModeLogic()
{
	CurrentAmmo -= 2;
	ShootTimer = 0.f;
	FirstWeapon->AI_Shoot();
	SecondWeapon->AI_Shoot();
}
