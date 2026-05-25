#include "RangerWeapon.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

ARangerWeapon::ARangerWeapon()
{
	Sounds.Add(ESound::RangerShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::RangerShoot]->SetAutoActivate(false);
	Sounds[ESound::RangerShoot]->SetupAttachment(Mesh);
}

void ARangerWeapon::AI_Shoot()
{
	PlaySound(ESound::RangerShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::RangerProjectile, false);
}

void ARangerWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	AssetsManager->PlaySound2D(ESound::RangerShoot);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(InProjectileName);
}

void ARangerWeapon::P_Reload()
{
	AssetsManager->PlaySound2D(ESound::RangerReload, true);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	if (AnimInstance)
	{
		Mesh->SetComponentTickEnabled(true);
		AnimInstance->PlayMontage(EWeaponAnimState::Reload);
	}
}

void ARangerWeapon::P_Setup()
{
	if (DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::RangerAbility1, EUpgradeAttribute::Special1)) AttachGrenadeLaunchSpecialVfx();
	else DelegateManager->OnSpecialUpgradeAdded.AddUObject(this, &ARangerWeapon::AddSpecialUpgradeLogic);
	Super::P_Setup();
}

float ARangerWeapon::P_UpgradeDamageAppliance()
{
	return P_ReceiveHitScanDamage() * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetRangerGrenadeSpecialBoost()
	+ P_ReceiveHitScanDamage() * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetRangerShieldSpecialBoost();
}

float ARangerWeapon::P_GetCriticalDamage()
{
	const float CriticalDamageMultiplier = P_Data->GetCriticalDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllCriticalMultiBonus();
	return P_Data->GetCriticalDamage() + CriticalDamageMultiplier;
}

void ARangerWeapon::BeginPlay()
{
	Name = EWeapon::RangerWeapon;
	Super::BeginPlay();
	Mesh->SetComponentTickInterval(1.f / 30.f);
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}

void ARangerWeapon::AddSpecialUpgradeLogic(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialAttributeName)
{
	if (InUpgradeName != EUpgrade::RangerAbility1 || InSpecialAttributeName != EUpgradeAttribute::Special1) return;
	DelegateManager->OnSpecialUpgradeAdded.RemoveAll(this);
	AttachGrenadeLaunchSpecialVfx();
}

void ARangerWeapon::AttachGrenadeLaunchSpecialVfx()
{
	GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetOnRangerAbilityStackStatusChangedDelegate().BindUObject(this, &ARangerWeapon::ChangeGrenadeLaunchSpecialComponentState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerGrenadeLaunchSpecial, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); },
	&GrenadeLaunchSpecialVfx, GetMesh(), FName("MuzzleSocket"), Proprietor->GetVariation(), true);
}

void ARangerWeapon::ChangeGrenadeLaunchSpecialComponentState(const bool bState) const
{
	if (!GrenadeLaunchSpecialVfx) return;
	if (bState) AssetsManager->ActivateWithVariation(EVfx::RangerGrenadeLaunchSpecial, nullptr, GrenadeLaunchSpecialVfx, false, Proprietor->GetVariation());
	else GrenadeLaunchSpecialVfx->Deactivate();
}
