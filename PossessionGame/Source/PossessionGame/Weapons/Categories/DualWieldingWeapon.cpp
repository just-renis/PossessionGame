#include "DualWieldingWeapon.h"
#include "OneHandedWeapon.h"
#include "PossessionGame/Data Assets/AI/Base Data/AIWeaponData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerWeaponData.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

ADualWieldingWeapon::ADualWieldingWeapon() { RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent"); }

void ADualWieldingWeapon::AI_Setup()
{
	Super::AI_Setup();
	CyclingState.bIsCycling = AI_Data->GetIsCyclingEnabled();
}

void ADualWieldingWeapon::AI_Shoot() { CyclingState.bIsCycling ? CyclingModeLogic() : NonCyclingModeLogic(); }

bool ADualWieldingWeapon::P_CanShoot()
{
	return !HandsAnimInstance->GetIsReloading() && GetCurrentAmmoAmount() > 0;
}

bool ADualWieldingWeapon::P_CanReload(int32 OtherAmmoSource)
{
	if (TotalAmmo == 0 && GetCurrentAmmoAmount() == 0) (void)DelegateManager->OnOutOfAmmo.ExecuteIfBound();
	return TotalAmmo > 0 && !HandsAnimInstance->GetIsReloading();
}

void ADualWieldingWeapon::P_Shoot()
{
	if (ShootTimer >= GetShootCooldown())
	{
		FirstWeapon->P_Shoot();
		SecondWeapon->P_Shoot();
		CurrentAmmo -= 2;
		PlayShootMontage();
		ShootTimer = 0.f;
		DelegateManager->OnWeaponAmmoChangeTwoParams.Broadcast(GetCurrentAmmoAmount(), TotalAmmo);
	}
}

void ADualWieldingWeapon::P_SetupTransfer(bool bSetToFirstHand)
{
	FirstWeapon->P_InitialSetupTransfer();
	FirstWeapon->P_AttachWeaponToHands(HandsMesh, "PlayerWeaponSocket1");
	SecondWeapon->P_InitialSetupTransfer();
	SecondWeapon->P_AttachWeaponToHands(HandsMesh, "PlayerWeaponSocket2");
	Super::P_SetupTransfer();
}

void ADualWieldingWeapon::P_UpdateAmmoTextHUD()
{
	if (!HandsAnimInstance->GetIsReloading()) DelegateManager->OnWeaponAmmoChangeTwoParams.Broadcast(GetCurrentAmmoAmount(), TotalAmmo);
}

void ADualWieldingWeapon::P_ChangeVfxVariation()
{
	FirstWeapon->P_ChangeVfxVariation();
	SecondWeapon->P_ChangeVfxVariation();
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

void ADualWieldingWeapon::PostProprietorSetup() { SetupHandsAnimInstance(); }

void ADualWieldingWeapon::P_SetVariationMaterial()
{
	FirstWeapon->P_SetVariationMaterial();
	SecondWeapon->P_SetVariationMaterial();
}

FVector ADualWieldingWeapon::GetSocketLocation(const FName InSocketName) const { return FirstWeapon->GetSocketLocation(InSocketName); }

FRotator ADualWieldingWeapon::GetSocketRotation(const FName InSocketName) const { return FirstWeapon->GetSocketRotation(InSocketName); }

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

void ADualWieldingWeapon::SetReverseActionSpeed(const float InActionSpeed)
{
	ReverseActionSpeed = InActionSpeed;
	FirstWeapon->SetReverseActionSpeed(InActionSpeed);
	SecondWeapon->SetReverseActionSpeed(InActionSpeed);
}

void ADualWieldingWeapon::MuzzleFlashLogic()
{
	FirstWeapon->MuzzleFlashLogic();
	SecondWeapon->MuzzleFlashLogic();
}
