#include "GuardianWeapon.h"
#include "GuardianSingularWeapon.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

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

void AGuardianWeapon::ToggleHandsMeshAbilityState(const bool bState) const
{
	if (!HandsMesh || !MI_Hands) return;
	Cast<AGuardianSingularWeapon>(FirstWeapon)->ToggleMeshAbilityState(bState);
	Cast<AGuardianSingularWeapon>(SecondWeapon)->ToggleMeshAbilityState(bState);
	MI_Hands->SetScalarParameterValue("Hue", bState ? 0.9f : 1.f);
	MI_Hands->SetScalarParameterValue("Emissive Effect", bState ? 1.f : 0.f);
	MI_Hands->SetScalarParameterValue("Width", bState ? 0.2f : 0.f);
}

bool AGuardianWeapon::P_CanShoot(const bool InbIsFirstWeapon) { return !HandsAnimInstance->IsPlaying(EHandsAnimState::Shoot); }

void AGuardianWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(-1); }

void AGuardianWeapon::P_Shoot(const bool bIsFirstWeapon)
{
	if (ShootTimer >= ShootCooldown)
	{
		PlayShootMontage();
		FirstWeapon->P_Shoot();
		GetWorldTimerManager().SetTimer(SecondWeaponShotHandle, this, &AGuardianWeapon::ShootSecondWeaponShot, 0.2f);
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
		HandsMesh->SetSkeletalMesh(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(InPrimaryDataAssetName)->HandsMesh.Get());
		if (!HandsMesh) return;
		MI_Hands = HandsMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (MI_Hands) MI_Hands->SetScalarParameterValue("Progress", 0.4f);
	}
}

void AGuardianWeapon::SetCustomDilation(const float InDilation)
{
	Super::SetCustomDilation(InDilation);
	FirstWeapon->SetCustomDilation(InDilation);
	SecondWeapon->SetCustomDilation(InDilation);
}

void AGuardianWeapon::ShootSecondWeaponShot() const { SecondWeapon->P_Shoot(); }

void AGuardianWeapon::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(SecondWeaponShotHandle);
	Super::DestroyThis();
}
