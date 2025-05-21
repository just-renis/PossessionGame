#include "GuardianWeapon.h"
#include "GuardianSingularWeapon.h"
#include "NiagaraComponent.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraVariationData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void AGuardianWeapon::PlayOverChargeVfx()
{
	if (FirstWeaponOverChargeComponent) FirstWeaponOverChargeComponent->Activate();
	else AssetsManager->AttachVfx(EVfx::GuardianOverCharge, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, FNiagaraVariationData(), &FirstWeaponOverChargeComponent, FirstWeapon->GetMesh(), "OverChargeSocket");
	if (SecondWeaponOverChargeComponent) SecondWeaponOverChargeComponent->Activate();
	else AssetsManager->AttachVfx(EVfx::GuardianOverCharge, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, FNiagaraVariationData(), &SecondWeaponOverChargeComponent, SecondWeapon->GetMesh(), "OverChargeSocket");
}

void AGuardianWeapon::StopOverChargeVfx() const
{
	if (FirstWeaponOverChargeComponent && SecondWeaponOverChargeComponent)
	{
		FirstWeaponOverChargeComponent->Deactivate();
		SecondWeaponOverChargeComponent->Deactivate();
	}
}

void AGuardianWeapon::CancelShootingHold() const
{
	Cast<AGuardianSingularWeapon>(FirstWeapon)->CancelShootingHold();
	Cast<AGuardianSingularWeapon>(SecondWeapon)->CancelShootingHold();
}

void AGuardianWeapon::ShootingHoldStarted() const
{
	Cast<AGuardianSingularWeapon>(FirstWeapon)->ShootingHoldStarted();
	Cast<AGuardianSingularWeapon>(SecondWeapon)->ShootingHoldStarted();
}

void AGuardianWeapon::ResetShootingHoldDuration() const
{
	Cast<AGuardianSingularWeapon>(FirstWeapon)->ResetShootingHoldDuration();
	Cast<AGuardianSingularWeapon>(SecondWeapon)->ResetShootingHoldDuration();
}

bool AGuardianWeapon::P_CanShoot(const bool InbIsFirstWeapon) { return !HandsAnimInstance->IsPlaying(EHandsAnimState::Shoot); }

void AGuardianWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(-1); }

void AGuardianWeapon::P_Shoot(const bool bIsFirstWeapon)
{
	FirstWeapon->P_Shoot();
	SecondWeapon->P_Shoot();
	if (ShootTimer >= ShootCooldown)
	{
		PlayShootMontage();
		ShootTimer = 0.f;
		DelegateManager->OnWeaponAmmoChangeThreeParams.Broadcast(FirstWeapon->GetCurrentAmmoAmount(), SecondWeapon->GetCurrentAmmoAmount(), TotalAmmo);
	}
}

void AGuardianWeapon::BeginPlay()
{
	Name = EWeapon::GuardianWeapon;
	Super::BeginPlay();
}

void AGuardianWeapon::SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh)
{
	const TObjectPtr<UWeaponManager> WeaponManager = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>();
	FirstWeapon = WeaponManager->GetWeapon(GetWorld(), EWeapon::GuardianSingularWeapon, CharacterMesh);
	SecondWeapon = WeaponManager->GetWeapon(GetWorld(), EWeapon::GuardianSingularWeapon, CharacterMesh, FName("AIWeaponSocket2"));
}

void AGuardianWeapon::SetMeshes(const EWeapon InPrimaryDataAssetName)
{
	if (!HandsMesh->GetSkeletalMeshAsset())
	{
		HandsMesh->SetSkeletalMesh(GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(InPrimaryDataAssetName)->HandsMesh.Get());
		if (!HandsMesh) return;
		if (const TObjectPtr<UMaterialInstanceDynamic> MI_Hands = HandsMesh->CreateAndSetMaterialInstanceDynamic(0)) MI_Hands->SetScalarParameterValue("Progress", 0.4f);
	}
}
