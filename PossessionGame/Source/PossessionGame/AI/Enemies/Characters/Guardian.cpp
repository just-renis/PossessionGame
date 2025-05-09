#include "Guardian.h"
#include "InputMappingContext.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/GuardianWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AGuardian::PlayOverChargeVfx() const { Cast<AGuardianWeapon>(Weapon)->PlayOverChargeVfx(); }

void AGuardian::StopOverChargeVfx() const { Cast<AGuardianWeapon>(Weapon)->StopOverChargeVfx(); }

AGuardian::AGuardian()
{
	Sounds.Add(ESound::GuardianSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::GuardianSpawn]->SetAutoActivate(false);
	Sounds[ESound::GuardianSpawn]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::GuardianDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::GuardianDeath]->SetAutoActivate(false);
	Sounds[ESound::GuardianDeath]->SetupAttachment(GetMesh());
}

void AGuardian::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::GuardianWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AGuardian::AI_PlayDeathSound() { PlaySound(ESound::GuardianDeath); }

void AGuardian::AI_PlaySpawnSound() { PlaySound(ESound::GuardianSpawn); }

void AGuardian::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &AGuardian::SetActionInMotion);
}

void AGuardian::P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	const ETriggerEvent TriggerEvent = EVariation::GuardianVariationGold == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(CharacterName) ? ETriggerEvent::Triggered : ETriggerEvent::Started;
	ShootBindingHandle = &LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, TriggerEvent, this, &AGuardian::P_Shoot, true);
}

void AGuardian::P_ChangeShootingInputEvent(const ECharacterType InCharacterType, const EVariation InVariationName)
{
	if (!EnhancedInputComponent || !PlayerMappingContext) return;
	if (ShootBindingHandle)
	{
		EnhancedInputComponent->RemoveBinding(*ShootBindingHandle);
		ShootBindingHandle = nullptr;
	}
	const ETriggerEvent TriggerEvent = EVariation::GuardianVariationGold == InVariationName ? ETriggerEvent::Triggered : ETriggerEvent::Started;
	ShootBindingHandle = &EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, TriggerEvent, this, &AGuardian::P_Shoot, true);
}

void AGuardian::BeginPlay()
{
	CharacterName = ECharacterType::Guardian;
	bCanReload = false;
	Super::BeginPlay();
	DelegateManager->OnVariationSelection.AddUObject(this, &AGuardian::P_ChangeShootingInputEvent);
}

void AGuardian::SetMesh(const ECharacterType PrimaryDataAssetName)
{
	Super::SetMesh(PrimaryDataAssetName);
	GetMesh()->SetMaterial(0, GuardianMaterial);
}

void AGuardian::DestroyThis()
{
	if (ShootBindingHandle) ShootBindingHandle = nullptr;
	DelegateManager->OnVariationSelection.RemoveAll(this);
	Super::DestroyThis();
}
