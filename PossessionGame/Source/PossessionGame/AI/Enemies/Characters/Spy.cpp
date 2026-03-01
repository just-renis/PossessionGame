#include "Spy.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Controllers/SpyAIController.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/SpyWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void ASpy::SetXrayState(const bool bState)
{
	bXrayEnabled = bState;
	if (!MID_Xray) MID_Xray = UMaterialInstanceDynamic::Create(MI_Xray, this);
	if (!MID_DarkXray) MID_DarkXray = UMaterialInstanceDynamic::Create(MID_DarkXray, this);
	if (bState)
	{
		AssetsManager->PlaySound2D(ESound::SpyXrayOn);
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyXrayEffect, MID_Xray, 1.f);
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyDarkXrayEffect, MID_DarkXray, 1.f);
		GetWorldTimerManager().SetTimer(ProcessXrayMaterialsHandle, this, &ASpy::ProcessXrayMaterialsActivation, 0.025f, true);
	}
	else
	{
		AssetsManager->PlaySound2D(ESound::SpyXrayOff);
		GetWorldTimerManager().SetTimer(ProcessXrayMaterialsHandle, this, &ASpy::ProcessXrayMaterialsDeactivation, 0.025f, true);
	}
}

void ASpy::StartTeleportProcess()
{
	if (bIsFirstAbilityActive) P_StopAbility(0);
	AssetsManager->PlaySound2D(ESound::SpyPlayerSnatchStart);
	bIsInvincible = true;
	bIsSecondAbilityActive = true;
	if (!TeleportLinesVfx) AssetsManager->AttachVfx(EVfx::SpyTeleportLines, [this]() -> bool { return GetIsAlive() && bIsSecondAbilityActive && bIsHijacked; }, &TeleportLinesVfx, Camera);
	else TeleportLinesVfx->Activate(true);
	SetTeleportMaterialValue(1.f);
}

void ASpy::FinishTeleportProcess()
{
	bIsSecondAbilityActive = false;
	AssetsManager->PlaySound2D(ESound::SpySnatchImpact);
	P_MakeCameraShake(ECameraShake::SpyTeleportKillCameraShake);
	Camera->SetWorldLocation(GetActorLocation() + FVector::UpVector * 60.f);
	bIsInvincible = false;
	if (TeleportLinesVfx) TeleportLinesVfx->DeactivateImmediate();
	SetTeleportMaterialValue(0.f);
}

void ASpy::SetFirstAbilityActiveState(const bool bState)
{
	bIsFirstAbilityActive = bState;
	bIsSlidePrimaryAnim = !bState;
	bIsJumpPrimaryAnim = !bState;
	Cast<ASpyWeapon>(Weapon)->SetInvincibilityState(bState);
	if (!MID_Invincibility) MID_Invincibility = UMaterialInstanceDynamic::Create(MI_Invincibility, this);
	if (bState)
	{
		AssetsManager->PlaySound2D(ESound::SpyInvincibilityOn);
		if (!InvincibilityVfx) AssetsManager->AttachVfx(EVfx::SpyInvincibility, [this]() -> bool { return GetIsAlive() && bIsFirstAbilityActive && bIsHijacked; }, &InvincibilityVfx, GetMesh(), FName("DeathSocket"));
		else InvincibilityVfx->Activate(true);
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyInvincibilityEffect, MID_Invincibility, 1.f);
		GetWorldTimerManager().SetTimer(ProcessInvincibilityMaterialHandle, this, &ASpy::ProcessInvincibilityMaterialActivation, 0.025f, true);
	}
	else
	{
		AssetsManager->PlaySound2D(ESound::SpyInvincibilityOff);
		BoostedAttacksAmount = static_cast<int32>(FirstAbilityAbsorbedDamage) / FirstAbilityAbsorbedAmountRequired;
		FirstAbilityAbsorbedDamage = 0.f;
		if (InvincibilityVfx) InvincibilityVfx->DeactivateImmediate();
		GetWorldTimerManager().SetTimer(ProcessInvincibilityMaterialHandle, this, &ASpy::ProcessInvincibilityMaterialDeactivation, 0.025f, true);
	}
}

ASpy::ASpy()
{
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::SpySpawn, SpawnSfx);
	DeathSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSfx"));
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::SpyDeath, DeathSfx);
	AI_DashSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("AI_DashSfx"));
	AI_DashSfx->SetAutoActivate(false);
	AI_DashSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::AI_SpyDash, AI_DashSfx);
}

void ASpy::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::SpyWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ASpy::AI_SetController() { AI = Cast<ASpyAIController>(GetController()); }

void ASpy::AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage)
{
	if (CanAbilityBeExecuted(1) && !IsActionsDisabled() && !bIsActionInMotion && CurrentHp <= MaxHp * 0.4f && CurrentHp > 0) AI_UseDashToLocationAbility();
	Super::AI_ReceivedDamage(InDamageReason, InDamage);
}

void ASpy::AI_DisableTeleportVfx() const
{
	if (!AI_TeleportVfx) return;
	AI_TeleportVfx->DeactivateImmediate();
}

void ASpy::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::SpyOnHijack); }

void ASpy::AI_PlaySpawnSound() { PlaySound(ESound::SpySpawn, true); }

void ASpy::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::SpyDeath);
}

void ASpy::AI_OnReachedNonEqsPoint()
{
	bIsSecondAbilityActive = false;
	SetIsAllowedToShoot(true);
	AI_DisableTeleportVfx();
}

void ASpy::AI_UseDashToLocationAbility()
{
	bIsSecondAbilityActive = true;
	PlaySound(ESound::AI_SpyDash, true);
	if (!AI_TeleportVfx) AssetsManager->AttachVfx(EVfx::AI_SpyTeleport, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed() && bIsSecondAbilityActive && !bIsHijacked; }, &AI_TeleportVfx, GetMesh(), FName("DeathSocket"));
	else AI_TeleportVfx->Activate(true);
	AI_UseAbility(1);
}

bool ASpy::P_IsSnatchAbilityAvailable() const { return CanAbilityBeExecuted(1); }

float ASpy::P_GetMeleeDamage()
{
	const float MeleeDamage = Super::P_GetMeleeDamage();
	if (IsBoostedAttacksAvailable())
	{
		SubtractBoostedAttack();
		return MeleeDamage + QuickMeleeAttackDamage * 0.3f;
	}
	return MeleeDamage;
}

float ASpy::P_GetStompDamage()
{
	const float StompAttackDamage = Super::P_GetStompDamage();
	if (IsBoostedAttacksAvailable())
	{
		SubtractBoostedAttack();
		return StompAttackDamage + StompDamage * 0.3f;
	}
	return StompAttackDamage;
}

void ASpy::P_Setup()
{
	bIsSecondAbilityActive = false;
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	DelegateManager->RequestIsSpyMarkerRequired.BindUObject(this, &ASpy::P_IsSnatchAbilityAvailable);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ASpy::SetIsActionInMotion);
}

void ASpy::P_SetupLeftClickInputStartedState()
{
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &ASpy::P_Shoot);
}

void ASpy::P_WeaponShoot()
{
	if (bIsFirstAbilityActive) P_StopAbility(0);
	Weapon->P_Shoot(EPoolObject::SpyProjectile);
}

void ASpy::P_MakeCameraShake(const ECameraShake InCameraShakeName)
{
	if (InCameraShakeName == ECameraShake::SpyTeleportKillCameraShake) UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(TeleportKillCameraShake);
	else Super::P_MakeCameraShake(InCameraShakeName);
}

void ASpy::P_ProcessMeleeAttack()
{
	if (bIsFirstAbilityActive) P_StopAbility(0);
	Super::P_ProcessMeleeAttack();
}

void ASpy::P_InitiateStomp()
{
	if (bIsFirstAbilityActive) P_StopAbility(0);
	Super::P_InitiateStomp();
}

void ASpy::P_RemoveReloadSound() { AssetsManager->RemoveStoredSound(ESound::SpyReload); }

void ASpy::P_FinishHijackProcess()
{
	DelegateManager->RequestIsSpyMarkerRequired.Unbind();
	Super::P_FinishHijackProcess();
}

void ASpy::P_ReceiveStunEffect(const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (bIsSecondAbilityActive) return;
	Super::P_ReceiveStunEffect(InEffectDuration, InDamage, InHitDirection);
}

float ASpy::ReceiveMovementSpeedModification() const
{
	if (!AI) return 1.f;
	if (!AI->GetIsUsingEQS() && !bIsHijacked) return AI_DashSpeedMultiplier;
	return 1.f;
}

void ASpy::BeginPlay()
{
	CharacterName = ECharacterType::Spy;
	Super::BeginPlay();
}

void ASpy::SetTeleportMaterialValue(const float InValue)
{
	if (!MID_Teleport) MID_Teleport = UMaterialInstanceDynamic::Create(MI_Teleport, this);
	if (InValue > 0.1f)
	{
		MID_Teleport->SetScalarParameterValue(FName("Density"), 2.3f);
		MID_Teleport->SetScalarParameterValue(FName("Intensity"), 1.8f);
		MID_Teleport->SetScalarParameterValue(FName("Radius"), 0.2f);
	}
	else
	{
		MID_Teleport->SetScalarParameterValue(FName("Density"), 0.f);
		MID_Teleport->SetScalarParameterValue(FName("Intensity"), 0.f);
		MID_Teleport->SetScalarParameterValue(FName("Radius"), 0.f);
	}
	GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyTeleportEffect, MID_Teleport, InValue);
}

void ASpy::ProcessXrayMaterialsActivation()
{
	if (!MID_Xray || !MID_DarkXray) return;
	XrayColorRadius -= 0.16f;
	XrayRadialRadius -= 0.03f;
	XrayDarkness -= 0.1f;
	MID_Xray->SetScalarParameterValue(FName("ColorRadius"), XrayColorRadius);
	MID_Xray->SetScalarParameterValue(FName("RadialRadius"), XrayRadialRadius);
	MID_DarkXray->SetScalarParameterValue(FName("Darkness"), XrayDarkness);
	if (++XrayCountLoops >= 10)
	{
		GetWorldTimerManager().ClearTimer(ProcessXrayMaterialsHandle);
		XrayCountLoops = 0;
	}
}

void ASpy::ProcessXrayMaterialsDeactivation()
{
	if (!MID_Xray || !MID_DarkXray) return;
	XrayColorRadius += 0.16f;
	XrayRadialRadius += 0.03f;
	XrayDarkness += 0.1f;
	MID_Xray->SetScalarParameterValue(FName("ColorRadius"), XrayColorRadius);
	MID_Xray->SetScalarParameterValue(FName("RadialRadius"), XrayRadialRadius);
	MID_DarkXray->SetScalarParameterValue(FName("Darkness"), XrayDarkness);
	if (++XrayCountLoops >= 10)
	{
		GetWorldTimerManager().ClearTimer(ProcessXrayMaterialsHandle);
		XrayCountLoops = 0;
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyXrayEffect, MID_Xray, 0.f);
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyDarkXrayEffect, MID_DarkXray, 0.f);
	}
}

void ASpy::ProcessInvincibilityMaterialActivation()
{
	if (!MID_Invincibility) return;
	InvincibilityColorRadius -= 0.16f;
	InvincibilityRadialRadius -= 0.03f;
	MID_Invincibility->SetScalarParameterValue(FName("ColorRadius"), InvincibilityColorRadius);
	MID_Invincibility->SetScalarParameterValue(FName("RadialRadius"), InvincibilityRadialRadius);
	if (++InvincibilityCountLoops >= 10)
	{
		GetWorldTimerManager().ClearTimer(ProcessInvincibilityMaterialHandle);
		InvincibilityCountLoops = 0;
	}
}

void ASpy::ProcessInvincibilityMaterialDeactivation()
{
	if (!MID_Invincibility) return;
	InvincibilityColorRadius += 0.16f;
	InvincibilityRadialRadius += 0.03f;
	MID_Invincibility->SetScalarParameterValue(FName("ColorRadius"), InvincibilityColorRadius);
	MID_Invincibility->SetScalarParameterValue(FName("RadialRadius"), InvincibilityRadialRadius);
	if (++InvincibilityCountLoops >= 10)
	{
		GetWorldTimerManager().ClearTimer(ProcessInvincibilityMaterialHandle);
		InvincibilityCountLoops = 0;
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyInvincibilityEffect, MID_Invincibility, 0.f);
	}
}

float ASpy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsFirstAbilityActive)
	{
		if (GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::SpyAbility1, EUpgradeAttribute::Special1)) FirstAbilityAbsorbedDamage += DamageAmount;
		return 0.f;
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
