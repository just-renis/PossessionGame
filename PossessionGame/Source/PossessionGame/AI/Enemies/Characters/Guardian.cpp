#include "Guardian.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"

EDamageReason AGuardian::GetDamageReason() const { return EDamageReason::GuardianDamage; }

FLinearColor AGuardian::GetDamageNumberColor() const { return bIsFirstAbilityInUse ? Super::GetDamageNumberColor() : FLinearColor::White; }

void AGuardian::AI_ToggleShieldVfx(const bool bState)
{
	AI_ToggleMeshShieldAbilityState(bState);
	ToggleShieldVfx(bState, EVfx::AI_GuardianShield);
}

void AGuardian::AI_ToggleMeshShieldAbilityState(const bool bState)
{
	if (!MID_EnemyAbilityShield) MID_EnemyAbilityShield = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	MID_EnemyAbilityShield->SetScalarParameterValue("Hue", bState ? 0.85f : 1.f);
	MID_EnemyAbilityShield->SetScalarParameterValue("Color Strength", bState ? 10.f : 1.f);
	MID_EnemyAbilityShield->SetScalarParameterValue("Emissive Effect", bState ? 1.f : 0.f);
	MID_EnemyAbilityShield->SetScalarParameterValue("Width", bState ? 0.2f : 0.f);
}

void AGuardian::P_LaunchVectorBlast()
{
	AssetsManager->PlaySound2D(ESound::VectorBlastLaunch);
	Weapon->ShootProjectile(EPoolObject::VectorBlastProjectile, true);
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("ShootSecondWeaponProjectile"), EPoolObject::VectorBlastProjectile, true, false);
	GetWorldTimerManager().SetTimer(ShootSecondWeaponProjectileHandle, TimerDel, 0.2f / CustomTimeDilation, false);
}

void AGuardian::SetCustomTimeDilation(const float InDilation)
{
	GetCharacterMovement()->SetComponentTickInterval(1.f / 60.f / InDilation);
	CustomTimeDilation = InDilation;
	GetWeapon()->SetCustomDilation(InDilation);
}

void AGuardian::ToggleShield(const bool bState)
{
	P_ToggleShieldVfx(bState);
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
	Sounds.Add(ESound::GuardianOverChargeStart, CreateDefaultSubobject<UAudioComponent>(TEXT("GuardianOverChargeStartSound")));
	Sounds[ESound::GuardianOverChargeStart]->SetAutoActivate(false);
	Sounds[ESound::GuardianOverChargeStart]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::GuardianOverChargeEnd, CreateDefaultSubobject<UAudioComponent>(TEXT("GuardianOverChargeEndSound")));
	Sounds[ESound::GuardianOverChargeEnd]->SetAutoActivate(false);
	Sounds[ESound::GuardianOverChargeEnd]->SetupAttachment(GetMesh());
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

void AGuardian::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::GuardianOnHijack); }

void AGuardian::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::GuardianDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void AGuardian::AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage)
{
	if (CanAbilityBeExecuted(0) && !IsActionsDisabled() && !bIsActionInMotion && CurrentHp <= MaxHp * 0.8f && CurrentHp > 0) AI_UseAbility(0);
	Super::AI_ReceivedDamage(InDamageReason, InDamage);
}

void AGuardian::P_Setup()
{
	bImmuneToFirstHit = false;
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	DelegateManager->RequestIsTimeStopUsed.BindUObject(this, &AGuardian::IsSecondAbilityInUse);
	Super::P_Setup();
	MID_Shield = UMaterialInstanceDynamic::Create(MI_Shield, this);
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &AGuardian::SetIsActionInMotion);
}

void AGuardian::P_SetupLeftClickInputStartedState()
{
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Triggered, this, &AGuardian::P_Shoot);
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
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::GuardianShield, MID_Shield, 0.f);
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

void AGuardian::P_ToggleShieldVfx(const bool bState) { ToggleShieldVfx(bState, EVfx::PlayerGuardianShield); }

void AGuardian::P_ShowReceivedDamageNumber(const float InDamage)
{
	if (bIsFirstAbilityInUse) (void)DelegateManager->OnPlayerDamageTaken.ExecuteIfBound(InDamage, FLinearColor::Blue);
	else Super::P_ShowReceivedDamageNumber(InDamage);
}

void AGuardian::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::GuardianProjectile); }

float AGuardian::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bImmuneToFirstHit)
	{
		bImmuneToFirstHit = false;
		return 0.f;
	}
	if (bReflectDamageEnabled && EventInstigator && EventInstigator->GetPawn())
	{
		UGameplayStatics::ApplyDamage(EventInstigator->GetPawn(), Damage, nullptr, this, UDamageType::StaticClass());
	}
	return Damage;
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

void AGuardian::DisableAllVfxAfterPossession()
{
	Super::DisableAllVfxAfterPossession();
	AI_ToggleShieldVfx(false);
	GuardianShieldComponent = nullptr;
}

void AGuardian::ToggleShieldVfx(const bool bState, const EVfx InVfxName)
{
	if (!GuardianShieldComponent)
	{
		if (!bState) return;
		GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(InVfxName, [this]() -> bool
		{ return IsAsyncAssetStillAllowedToBeUsed(); }, &GuardianShieldComponent, GetMesh(), FName("DeathSocket"));
	}
	else
	{
		if (bState) GuardianShieldComponent->Activate(true);
		else GuardianShieldComponent->DeactivateImmediate();
	}
}

void AGuardian::InitiateDeath()
{
	if (bIsHijacked) DelegateManager->RequestIsTimeStopUsed.Unbind();
	Super::InitiateDeath();
}

void AGuardian::TurnOffShieldPostProcess() const
{
	if (MID_Shield) GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::GuardianShield, MID_Shield, 0.f);
}

void AGuardian::DestroyImmediately()
{
	TurnOffShieldPostProcess();
	Super::DestroyImmediately();
}

void AGuardian::ShootSecondWeaponProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, bool bFromFirstWeapon)
{
	AssetsManager->PlaySound2D(ESound::VectorBlastLaunch);
	Weapon->ShootProjectile(InProjectileName, bIsFromAbility, NAME_None, bFromFirstWeapon);
}
