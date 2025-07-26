#include "TricksterWeapon.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

ATricksterWeapon::ATricksterWeapon()
{
	Sounds.Add(ESound::TricksterReload, CreateDefaultSubobject<UAudioComponent>(TEXT("reloadSound")));
	Sounds[ESound::TricksterReload]->SetAutoActivate(false);
	Sounds[ESound::TricksterReload]->SetupAttachment(RootComponent);
}

void ATricksterWeapon::AI_Reload() { PlaySound(ESound::TricksterReload, true); }

void ATricksterWeapon::P_Reload(int32 OtherAmmoSource)
{
	AssetsManager->PlaySound2D(ESound::TricksterReload);
	P_PlayOverChargeVfx();
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	Super::P_Reload(OtherAmmoSource);
}

void ATricksterWeapon::P_PlayOverChargeVfx()
{
	if (FirstWeaponOverChargeComponent) FirstWeaponOverChargeComponent->Activate();
	else AssetsManager->AttachVfx(EVfx::TricksterReload, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed() && Proprietor->GetWeapon()->GetHandsAnimInstance()->GetIsReloading(); }, &FirstWeaponOverChargeComponent, FirstWeapon->GetMesh(), "OverChargeSocket");
	if (SecondWeaponOverChargeComponent) SecondWeaponOverChargeComponent->Activate();
	else AssetsManager->AttachVfx(EVfx::TricksterReload, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed() && Proprietor->GetWeapon()->GetHandsAnimInstance()->GetIsReloading(); }, &SecondWeaponOverChargeComponent, SecondWeapon->GetMesh(), "OverChargeSocket");
}

void ATricksterWeapon::P_StopOverChargeVfx() const
{
	if (FirstWeaponOverChargeComponent && SecondWeaponOverChargeComponent)
	{
		FirstWeaponOverChargeComponent->Deactivate();
		SecondWeaponOverChargeComponent->Deactivate();
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

void ATricksterWeapon::PlayShootMontage()
{
	if (ShootMontageHandler.bIsFirstWeaponShooting && ShootMontageHandler.bIsSecondWeaponShooting) HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	else if (ShootMontageHandler.bIsFirstWeaponShooting) HandsAnimInstance->PlayMontage(EHandsAnimState::ShootLeft);
	else HandsAnimInstance->PlayMontage(EHandsAnimState::ShootRight);
}

void ATricksterWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bFromFirstWeapon)
{
	if (bFromFirstWeapon) FirstWeapon->ShootProjectile(InProjectileName);
	else SecondWeapon->ShootProjectile(InProjectileName);
}
