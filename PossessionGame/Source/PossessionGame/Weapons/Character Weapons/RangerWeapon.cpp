#include "RangerWeapon.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

ARangerWeapon::ARangerWeapon()
{
	Sounds.Add(ESound::RangerShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::RangerShoot]->SetAutoActivate(false);
	Sounds[ESound::RangerShoot]->SetupAttachment(Mesh);
	Sounds.Add(ESound::RangerReload, CreateDefaultSubobject<UAudioComponent>(TEXT("reloadSound")));
	Sounds[ESound::RangerReload]->SetAutoActivate(false);
	Sounds[ESound::RangerReload]->SetupAttachment(Mesh);
}

void ARangerWeapon::AI_Reload() { PlaySound(ESound::RangerReload); }

void ARangerWeapon::AI_Shoot()
{
	PlaySound(ESound::RangerShoot);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::RangerProjectile);
}

void ARangerWeapon::P_Shoot(bool bIsFirstWeapon)
{
	PlaySound(ESound::RangerShoot);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(bIsFirstWeapon);
}

void ARangerWeapon::P_Reload(int32 OtherAmmoSource)
{
	PlaySound(ESound::RangerReload);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	if (AnimInstance) AnimInstance->PlayMontage(EWeaponAnimState::Reload);
	Super::P_Reload(OtherAmmoSource);
}

void ARangerWeapon::P_ApplyCurrentVariation()
{
	if (EVariation::RangerVariationBlue == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(Proprietor->GetCharacterName())) HitScanDamage = DefaultHitScanDamage *= 0.6f;
}

void ARangerWeapon::BeginPlay()
{
	Name = EWeapon::RangerWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}
