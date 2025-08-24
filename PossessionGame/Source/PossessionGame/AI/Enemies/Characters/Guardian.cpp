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
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"

void AGuardian::SetCustomTimeDilation(const float InDilation)
{
	if (FMath::IsNearlyEqual(InDilation, 5.f, 0.1f))
	{
		AssetsManager->AttachVfx(EVfx::StopTimeAbility, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); },nullptr, Cast<USceneComponent>(GetCamera()), NAME_None, false, FVector(20.f, 0.f, 0.f), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}
	else AssetsManager->AttachVfx(EVfx::StopTimeAbilityReverse, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, nullptr, Cast<USceneComponent>(GetCamera()), NAME_None, false, FVector(20.f, 0.f, 0.f), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	CustomTimeDilation = InDilation;
	GetWeapon()->SetCustomDilation(InDilation);
}

void AGuardian::ToggleShield(const bool bState)
{
	if (bState)
	{
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::GuardianShield, MID_Shield, 1.f);
		GetWorldTimerManager().SetTimer(ShieldUpdateHandle, this, &AGuardian::P_UpdateShieldMaterialAppear, 0.025f, true);
	}
	else
	{
		bIsShieldDisappearing = true;
		GetWorldTimerManager().SetTimer(ShieldUpdateHandle, this, &AGuardian::P_UpdateShieldMaterialDisappear, 0.025f, true);
	}
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
	AssetsManager->AttachVfx(EVfx::GuardianDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void AGuardian::P_Setup()
{
	bImmuneToFirstHit = false;
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	MID_Shield = UMaterialInstanceDynamic::Create(MI_Shield, this);
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &AGuardian::SetIsActionInMotion);
}

void AGuardian::P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Triggered, this, &AGuardian::P_Shoot);
	if (EVariation::GuardianVariationGold == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(CharacterName))
	{
		LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &AGuardian::P_OnShootStarted);
	}
}

void AGuardian::P_SetupLeftClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	if (EVariation::GuardianVariationGold != GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(CharacterName)) return;
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Completed, this, &AGuardian::P_CancelShootingHold);
}

void AGuardian::P_OnShootStarted() { Cast<AGuardianWeapon>(Weapon)->P_ShootingHoldStarted(); }

void AGuardian::P_ResetShootHoldDuration() const { Cast<AGuardianWeapon>(Weapon)->P_ResetShootingHoldDuration(); }

void AGuardian::P_CancelShootingHold() { Cast<AGuardianWeapon>(Weapon)->P_CancelShootingHold(); }

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

void AGuardian::P_Move(const FInputActionValue& Value)
{
	Super::P_Move(Value);
	if (Variation == EVariation::GuardianVariationGold) P_ResetShootHoldDuration();
}

void AGuardian::P_UpdateShieldMaterialAppear()
{
	if (bIsShieldDisappearing) P_ResetShieldValues();
	P_UpdateShieldMaterial(true);
	if (ShieldColorDensity <= 2.5f) GetWorldTimerManager().ClearTimer(ShieldUpdateHandle);
}

void AGuardian::P_UpdateShieldMaterialDisappear()
{
	P_UpdateShieldMaterial(false);
	if (ShieldColorDensity >= 5.f)
	{
		bIsShieldDisappearing = false;
		GetWorldTimerManager().ClearTimer(ShieldUpdateHandle);
	}
}

void AGuardian::P_UpdateShieldMaterial(const bool bState)
{
	ShieldColorDensity += bState ? -0.25f : 0.25f;
	ShieldRadialRadius += bState ? -0.045f : 0.045f;
	Radius += bState ? -0.2f : 0.2f;
	MID_Shield->SetScalarParameterValue("ColorDensity", ShieldColorDensity);
	MID_Shield->SetScalarParameterValue("RadialRadius", ShieldRadialRadius);
	MID_Shield->SetScalarParameterValue("Radius", Radius);
}

void AGuardian::P_ResetShieldValues()
{
	ShieldColorDensity = 5.f;
	ShieldRadialRadius = 1.f;
	Radius = 3.f;
	MID_Shield->SetScalarParameterValue("ColorDensity", ShieldColorDensity);
	MID_Shield->SetScalarParameterValue("RadialRadius", ShieldRadialRadius);
	MID_Shield->SetScalarParameterValue("Radius", Radius);
	bIsShieldDisappearing = false;
}

float AGuardian::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bImmuneToFirstHit)
	{
		bImmuneToFirstHit = false;
		return 0.f;
	}
	if (EventInstigator && bReflectDamageEnabled) UGameplayStatics::ApplyDamage(EventInstigator->GetPawn(), DamageAmount * 0.5f, nullptr, this, UDamageType::StaticClass());
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AGuardian::BeginPlay()
{
	CharacterName = ECharacterType::Guardian;
	bCanReload = false;
	Super::BeginPlay();
	if (GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::GuardianAbility1, EUpgradeAttribute::Special1)) bImmuneToFirstHit = true;
}

void AGuardian::SetMesh(const ECharacterType PrimaryDataAssetName)
{
	Super::SetMesh(PrimaryDataAssetName);
	GetMesh()->SetMaterial(0, MI_Guardian);
}
