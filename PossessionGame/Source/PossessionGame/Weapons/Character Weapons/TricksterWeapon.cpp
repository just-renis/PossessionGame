#include "TricksterWeapon.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void ATricksterWeapon::P_Reload()
{
	AssetsManager->PlaySound2D(ESound::TricksterReload, true);
	P_PlayOverChargeVfx();
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
}

void ATricksterWeapon::P_PlayOverChargeVfx()
{
	if (FirstWeaponOverChargeVfx) AssetsManager->ActivateWithVariation(EVfx::TricksterReload, nullptr, FirstWeaponOverChargeVfx, false, Proprietor->GetVariation());
	else AssetsManager->AttachVfx(EVfx::TricksterReload, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed() && Proprietor->GetWeapon()->GetHandsAnimInstance()->GetIsReloading(); }, &FirstWeaponOverChargeVfx, FirstWeapon->GetMesh(), FName("OverChargeSocket"), Proprietor->GetVariation());
	if (SecondWeaponOverChargeVfx) AssetsManager->ActivateWithVariation(EVfx::TricksterReload, nullptr, SecondWeaponOverChargeVfx, false, Proprietor->GetVariation());
	else AssetsManager->AttachVfx(EVfx::TricksterReload, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed() && Proprietor->GetWeapon()->GetHandsAnimInstance()->GetIsReloading(); }, &SecondWeaponOverChargeVfx, SecondWeapon->GetMesh(), FName("OverChargeSocket"), Proprietor->GetVariation());
}

void ATricksterWeapon::P_StopOverChargeVfx() const
{
	if (FirstWeaponOverChargeVfx && SecondWeaponOverChargeVfx)
	{
		FirstWeaponOverChargeVfx->Deactivate();
		SecondWeaponOverChargeVfx->Deactivate();
	}
}

void ATricksterWeapon::BeginPlay()
{
	Name = EWeapon::TricksterWeapon;
	Super::BeginPlay();
}

void ATricksterWeapon::SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh)
{
	const TObjectPtr<UWeaponManager> WeaponManager = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>();
	FirstWeapon = WeaponManager->GetWeapon(GetWorld(), EWeapon::TricksterSingularWeapon, CharacterMesh);
	SecondWeapon = WeaponManager->GetWeapon(GetWorld(), EWeapon::TricksterSingularWeapon, CharacterMesh, FName("AIWeaponSocket2"));
}

void ATricksterWeapon::SetMeshes(const EWeapon InPrimaryDataAssetName)
{
	if (!HandsMesh->GetSkeletalMeshAsset()) HandsMesh->SetSkeletalMesh(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(InPrimaryDataAssetName)->HandsMesh.Get());
}

void ATricksterWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, const bool bFromFirstWeapon)
{
	if (bFromFirstWeapon) FirstWeapon->ShootProjectile(InProjectileName, bIsFromAbility);
	else SecondWeapon->ShootProjectile(InProjectileName, bIsFromAbility);
}
