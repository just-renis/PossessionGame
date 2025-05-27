#include "Guardian.h"
#include "InputMappingContext.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/GuardianWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "Camera/CameraComponent.h"

void AGuardian::SetCustomTimeDilation(const float InDilation)
{
	if (FMath::IsNearlyEqual(InDilation, 5.f, 0.1f))
	{
		AssetsManager->AttachVfx(EVfx::StopTimeAbility, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, FNiagaraVariationData(), nullptr, Cast<USceneComponent>(GetCamera()), NAME_None, false, TMap<FName, FNiagaraAttribute>(), FVector(20.f, 0.f, 0.f), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}
	else AssetsManager->AttachVfx(EVfx::StopTimeAbilityReverse, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, FNiagaraVariationData(), nullptr, Cast<USceneComponent>(GetCamera()), NAME_None, false, TMap<FName, FNiagaraAttribute>(), FVector(20.f, 0.f, 0.f), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	CustomTimeDilation = InDilation;
	GetWeapon()->CustomTimeDilation = InDilation;
}

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

void AGuardian::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::GuardianDeath);
}

void AGuardian::AI_PlaySpawnSound() { PlaySound(ESound::GuardianSpawn, true); }

void AGuardian::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::GuardianDeathElectrification, nullptr, FNiagaraVariationData(), nullptr, GetMesh(), "DeathSocket");
}

void AGuardian::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &AGuardian::SetActionInMotion);
}

void AGuardian::P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	const ETriggerEvent TriggerEvent = EVariation::GuardianVariationGold == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(CharacterName) ? ETriggerEvent::Triggered : ETriggerEvent::Started;
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, TriggerEvent, this, &AGuardian::P_Shoot, true);
	if (TriggerEvent == ETriggerEvent::Triggered)
	{
		LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &AGuardian::P_OnShootStarted);
	}
}

void AGuardian::P_SetupLeftClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	if (EVariation::GuardianVariationGold != GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(CharacterName)) return;
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Completed, this, &AGuardian::P_CancelShootingHold);
}

void AGuardian::P_OnShootStarted() { Cast<AGuardianWeapon>(Weapon)->ShootingHoldStarted(); }

void AGuardian::P_ResetShootHoldDuration() const { Cast<AGuardianWeapon>(Weapon)->ResetShootingHoldDuration(); }

void AGuardian::P_CancelShootingHold() { Cast<AGuardianWeapon>(Weapon)->CancelShootingHold(); }

void AGuardian::P_InitiateJump()
{
	Super::P_InitiateJump();
	if (Variation == EVariation::GuardianVariationGold) P_ResetShootHoldDuration();
}

void AGuardian::P_InitiateStomp()
{
	Super::P_InitiateStomp();
	if (Variation == EVariation::GuardianVariationGold) P_ResetShootHoldDuration();
}

void AGuardian::P_InitiateDash()
{
	Super::P_InitiateDash();
	if (Variation == EVariation::GuardianVariationGold) P_ResetShootHoldDuration();
}

void AGuardian::P_Move(const FInputActionValue& Value)
{
	Super::P_Move(Value);
	if (Variation == EVariation::GuardianVariationGold) P_ResetShootHoldDuration();
}

void AGuardian::BeginPlay()
{
	CharacterName = ECharacterType::Guardian;
	bCanReload = false;
	Super::BeginPlay();
}

void AGuardian::SetMesh(const ECharacterType PrimaryDataAssetName)
{
	Super::SetMesh(PrimaryDataAssetName);
	GetMesh()->SetMaterial(0, MI_Guardian);
}
