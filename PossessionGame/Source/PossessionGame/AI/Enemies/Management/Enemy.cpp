#include "Enemy.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyAIController.h"
#include "EnemyAnimationInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/Data Assets/AI/Base Data/AICharacterData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerCharacterData.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EDisablingEffect.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Enums/EPlayerLevelUpgradeId.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EUIBar.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Enums/EWarningIndicator.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/UI/MainWidgets/DeathScreenUI.h"
#include "PossessionGame/UI/MainWidgets/DemoEndUI.h"
#include "PossessionGame/UI/MainWidgets/HijackUI.h"
#include "PossessionGame/UI/MainWidgets/PauseMenuUI.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/UI/MainWidgets/PlayerLevelSystemUI.h"
#include "PossessionGame/UI/MainWidgets/SettingsUI.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/UI/MainWidgets/VariationsUI.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/UI/SecondaryWidgets/AIHpBarUI.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameGameModeBase.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/SettingsSave.h"
#include "PossessionGame/Utilities/Subsystems/AbilityManager.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"
#include "UserSettings/EnhancedInputUserSettings.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;
	Camera->SetActive(false);
	AI_HpBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AI_HpBar"));
	AI_HpBarComponent->SetupAttachment(RootComponent);
	AI_HpBarComponent->SetDrawSize(FVector2D(1920.f, 1080.f));
	AI_HpBarComponent->SetRelativeScale3D(FVector(1.f, 0.05f, 0.01f));
	Sounds.Add(ESound::MechanicalDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("mechanicaldeathSound")));
	Sounds[ESound::MechanicalDeath]->SetAutoActivate(false);
	Sounds[ESound::MechanicalDeath]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::EmpEffectApplied, CreateDefaultSubobject<UAudioComponent>(TEXT("empEffectAppliedSound")));
	Sounds[ESound::EmpEffectApplied]->SetAutoActivate(false);
	Sounds[ESound::EmpEffectApplied]->SetupAttachment(GetMesh());
	VectorBlastSpecialImpactUpgrade = CreateDefaultSubobject<UAudioComponent>(FName("VectorBlastSpecialImpactUpgrade"));
	VectorBlastSpecialImpactUpgrade->SetAutoActivate(false);
	VectorBlastSpecialImpactUpgrade->SetupAttachment(GetMesh());
	WraithBurnExplosionSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("WraithBurnExplosionSfx"));
	WraithBurnExplosionSfx->SetAutoActivate(false);
	WraithBurnExplosionSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::VectorBlastSpecialImpactUpgrade, VectorBlastSpecialImpactUpgrade);
	Sounds.Add(ESound::RangerGrenadeExplosion, WraithBurnExplosionSfx);
}

bool AEnemy::GetIsHijackAble() const { return bIsHijackAble; }

bool AEnemy::CanAbilityBeExecuted(const int32 AbilityIndex) const
{
	if (const TObjectPtr<UAbility>* Ability = Abilities.Find(AbilityIndex)) return Ability->Get()->CanExecute();
	return false;
}

void AEnemy::AI_InitiateShooting() { if (CurrentMoveMode != EMoveMode::Run && !AnimInstance->IsPlaying(EEnemyAnimState::Reload)) AI_PlayShootMontage(); }

void AEnemy::AI_ShootLogic()
{
	if (bIsInputLocked || bIsActionInMotion || !bCanShoot || AnimInstance->IsPlaying(EEnemyAnimState::Reload) || CurrentMoveMode == EMoveMode::Run) return;
	AI_Shoot();
}

void AEnemy::AI_ReloadLogic()
{
	if (bIsInputLocked || bIsActionInMotion || !Weapon->GetIsReloadEnabled() || !bCanReload) return;
	AI_Reload();
}

void AEnemy::AI_UseAbility(const int32 InAbilityIndex) const
{
	if (const TObjectPtr<UAbility>* Ability = Abilities.Find(InAbilityIndex)) Ability->Get()->AI_Execute();
}

void AEnemy::AI_SetMoveMode(const EMoveMode InMoveMode) { SetMaxWalkSpeed(InMoveMode); }

void AEnemy::AI_SetShootPause()
{
	bShootPauseActive = true;
	GetWorldTimerManager().SetTimer(AI_ClearShootPauseHandle, this, &AEnemy::AI_ClearShootPause, AI_GetShootPause(), false);
}

void AEnemy::AI_SetCanShoot(const bool bState)
{
	bCanShoot = bState;
	if (!bCanShoot)
	{
		Weapon->SubtractShootCycleUntilReload();
		Weapon->GetShootCyclesUntilReload() <= 0 ? AI_ReloadLogic() : AI_SetShootPause();
	}
}

void AEnemy::AI_Disable() { if (AI) AI->StopLogic(true); }

void AEnemy::AI_StopAbility(const int32 InIndex) const { if (const TObjectPtr<UAbility>* Ability = Abilities.Find(InIndex)) Ability->Get()->AI_StopAbility(); }

void AEnemy::AI_SetupUpgradesAppliance()
{
	MaxHp += SaveSlotManager->GetRunSaveSlot()->GetHpUpgradesForEnemy(CharacterName);
	CurrentHp = MaxHp;
}

void AEnemy::AI_SetupShootLogic()
{
	if (!Weapon) return;
	GetWorld()->GetTimerManager().SetTimer(AI_ShootLogicHandle, this, &AEnemy::AI_MainShootLogic, 1.f / 60.f, true);
}

void AEnemy::AI_TriggerVectorBlastSpecialImpact()
{
	PlaySound(ESound::VectorBlastSpecialImpactUpgrade);
	if (!VectorBlastSpecialImpactVfx) AssetsManager->AttachVfx(EVfx::GuardianVectorBlastUpgrade, [this]() -> bool { return GetIsAlive(); }, &VectorBlastSpecialImpactVfx, GetMesh(), FName("DeathSocket"));
	else VectorBlastSpecialImpactVfx->Activate();
}

void AEnemy::AI_SetupRespawn()
{
	if (!bAIRespawnRequested) return;
	MaxHp = AI_Data->GetMaxHp() + SaveSlotManager->GetRunSaveSlot()->GetHpUpgradesForEnemy(CharacterName);
	CurrentHp = MaxHp * SavedRespawnHpPercentage;
	MeshBaseMaterialCondenseValue = 0.4f;
	MeshEmissiveMaterialOpacityValue = 0.5f;
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		if (GetMesh()->GetMaterial(i)->GetName().Contains("Emissive")) MID_MeshEmissiveMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
		else MI_MeshBaseMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
	}
	if (MI_MeshBaseMaterial) MI_MeshBaseMaterial->SetScalarParameterValue("Progress", MeshBaseMaterialCondenseValue);
	if (MID_MeshEmissiveMaterial) MID_MeshEmissiveMaterial->SetScalarParameterValue("Opacity", MeshEmissiveMaterialOpacityValue);
}

void AEnemy::AI_ProcessSpawnEffects()
{
	AssetsManager->SpawnVfx(AssetsManager->GetCharacterVfxName(CharacterName, EVfxType::Spawn), [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, GetMesh()->GetSocketLocation("SpawnSocket"));
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		if (GetMesh()->GetMaterial(i)->GetName().Contains("Emissive")) MID_MeshEmissiveMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
		else MI_MeshBaseMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
	}
	MI_MeshBaseMaterial->SetScalarParameterValue("Progress", 0.7f);
	GetWorldTimerManager().SetTimer(SpawnCondenseMaterialsProcessHandle, this, &AEnemy::SpawnCondenseMaterialsProcess, 0.05f, true);
}

void AEnemy::AI_SetRespawnRequest(const float InHpPercentage, const bool bIsCorrupted)
{
	bAIRespawnRequested = true;
	SavedRespawnHpPercentage = InHpPercentage;
	if (!bIsCorrupted) return;
	AssetsManager->AttachVfx(EVfx::Corrupted, [this]() -> bool { return GetIsAlive(); }, &CorruptedVfx, GetMesh(), FName("DeathSocket"));
	bIsHijackAble = false;
}

void AEnemy::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (InDamage > 0.f && !bHitByOtherAI) ReceiveDamage(InDamage, EDamageReason::BreacherBashDamage, nullptr, nullptr);
	if (!InHitDirection.ContainsNaN())
	{
		InHitDirection.Z = 0.f;
		GetCharacterMovement()->StopMovementImmediately();
		LaunchCharacter(InHitDirection * (AI_Data ? AI_Data->GetBashImpulseForce() : 5000.f), true, true);
	}
	ApplyDisablingActionEffect(InEffectDuration, EDisablingEffect::Stun);
}

void AEnemy::AI_StunEffectEnded()
{
	bAffectedByStun = false;
	RemoveDisablingActionEffect();
}

void AEnemy::AI_TriggerGrenadeProjection(const EPoolObject InProjectileName, const EVfx InMarkerVfxName)
{
	FProjectileData* ProjectileData = GetWeapon()->GetProjectileData(InProjectileName);
	if (!ProjectileData) return;
	const int32 ProjectileAmount = 1 + GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(InProjectileName, EUpgradeAttribute::ProjectilesAmount, false, true);
	const FAdditionalProjectiles AdditionalProjectilesData = ProjectileData->GetMultiProjectileData(ProjectileAmount);
	for (int32 i = 0; i < ProjectileAmount; i++)
	{
		FPredictProjectilePathParams PathParams;
		PathParams.StartLocation = GetWeapon()->GetSocketLocation(ProjectileData->ProjectileSocketName);
		const FRotator BaseRotation = GetWeapon()->GetSocketRotation(ProjectileData->ProjectileSocketName);
		const FVector FinalDirection = BaseRotation.RotateVector(AdditionalProjectilesData.Data[i].Direction);
		PathParams.LaunchVelocity = FinalDirection.GetSafeNormal() * AdditionalProjectilesData.Data[i].MaxSpeed;
		PathParams.bTraceWithCollision = true;
		PathParams.ProjectileRadius = 1.f;
		PathParams.MaxSimTime = 8.f;
		PathParams.SimFrequency = 30.f;
		PathParams.OverrideGravityZ = GetWorld()->GetGravityZ() * AdditionalProjectilesData.Data[i].GravityScale;
		PathParams.TraceChannel = ECC_GameTraceChannel6;
		FPredictProjectilePathResult PathResult;
		if (UGameplayStatics::PredictProjectilePath(GetWorld(), PathParams, PathResult))
		{
			FRotator MarkerRotation = PathResult.HitResult.ImpactNormal.Rotation();
			MarkerRotation.Pitch -= 90.f; 
			AssetsManager->SpawnVfx(InMarkerVfxName, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, PathResult.HitResult.Location, MarkerRotation);
		}
	}
}

void AEnemy::AI_SetHijackOutline(const bool bState)
{
	bIsLookedAtByPlayer = bState;
	AI_SetSpySnatchMarker(bState);
	if (bState)
	{
		GetMesh()->SetOverlayMaterial(HijackOutlineMaterial);
		AI_ShowHpBar();
		if (!HijackableVfx)
		{
			if (AssetsManager->GetVfxData(EVfx::HijackViable).GetIsInLoad() && !AssetsManager->GetVfxData(EVfx::HijackViable).GetIsLoaded()) return;
			AssetsManager->AttachVfx(EVfx::HijackViable, [this]() -> bool { return GetIsAlive() && !bIsHijacked && bIsLookedAtByPlayer; }, &HijackableVfx, GetMesh(), FName("RootSocket"));
		}
		else if (!HijackableVfx->IsActive()) HijackableVfx->Activate();
		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &AEnemy::AI_SetHijackOutline, false);
		GetWorldTimerManager().SetTimer(AI_DisableHijackOutlineHandle, TimerDel, 0.016f, false);
	}
	else
	{
		GetMesh()->SetOverlayMaterial(nullptr);
		AI_HideHpBar();
		DisableHijackViableVfx();
	}
}

void AEnemy::AI_SetUnHijackOutline(const bool bState)
{
	bIsLookedAtByPlayer = bState;
	AI_SetSpySnatchMarker(bState);
	if (bState)
	{
		AI_ShowHpBar();
		GetMesh()->SetOverlayMaterial(UnHijackOutlineMaterial);
		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &AEnemy::AI_SetUnHijackOutline, false);
		GetWorldTimerManager().SetTimer(AI_DisableUnHijackOutlineHandle, TimerDel, 0.016f, false);
	}
	else GetMesh()->SetOverlayMaterial(nullptr);
}

void AEnemy::AI_ReduceHalfHp() { ReceiveDamage(MaxHp * 0.5f, EDamageReason::None, nullptr, nullptr); }

void AEnemy::AI_ReceiveEmpEffect(const float InEffectDuration) { ApplyDisablingActionEffect(InEffectDuration, EDisablingEffect::Emp); }

void AEnemy::AI_SetAcoEnemy()
{
	bIsAco = true;
	bIsHijackAble = false;
	AI_SetCombatState(false);
	(void)DelegateManager->OnAcoEnemySpawned.ExecuteIfBound(this);
}

void AEnemy::AI_ApplyAcoHijack()
{
	AssetsManager->SpawnVfx(EVfx::AcoHijackEffect, nullptr, GetSocketLocation(FName("RootSocket")), GetSocketRotation(FName("RootSocket")), EVariation::Default,
		{ { FName("Skeletal Mesh"), FNiagaraAttribute(EUserParamType::Object, GetMesh()) } });
	AI_AcoMaterialChange();
	AI_SetCombatState(true);
}

void AEnemy::P_ReEnableRunIfToggleMode()
{
	if (!bPlayerRunModeIsToggle || !bWasRunningBeforeShooting) return;
	bWasRunningBeforeShooting = false;
	P_ToggleRun(true);
}

void AEnemy::P_ApplyUpgradeOnSelection()
{
	bInUpgradeSelection = false;
	for (const TPair<int32, TObjectPtr<UAbility>> Ability : Abilities) Ability.Value->SetupUpgradeAppliance();
}

void AEnemy::P_UseAbility(const int32 InAbilityIndex)
{
	if (bIsInputLocked || bIsActionInMotion) return;
	if (const TObjectPtr<UAbility>* Ability = Abilities.Find(InAbilityIndex))
	{
		if (Ability->Get()->CanExecute()) Ability->Get()->P_InitiateExecute();
	}
}

void AEnemy::P_UpdateAmmoHUD(const int32 WeaponAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(WeaponAmmo); }

void AEnemy::P_UpdateAmmoHUD(const int32 WeaponAmmo, const int32 MaxAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(WeaponAmmo, MaxAmmo); }

void AEnemy::P_UpdateAmmoHUD(const int32 FirstWeaponAmmo, const int32 SecondWeaponAmmo, const int32 MaxAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(FirstWeaponAmmo, SecondWeaponAmmo, MaxAmmo); }

void AEnemy::P_SetFov(const float InFov)
{
	TargetFov = InFov;
	FovChangeTick = (TargetFov - Camera->FieldOfView) * 0.1f;
	GetWorldTimerManager().SetTimer(P_ContinuousFovChangeHandle, this, &AEnemy::P_ContinuousFovChange, 0.025f, true);
}

void AEnemy::P_IncreaseMovementSpeed(const float InPercentage) { SetMaxWalkSpeed(CurrentMoveMode, P_Data->GetMaxWalkSpeed() + P_Data->GetMaxWalkSpeed() * InPercentage * 0.01f); }

void AEnemy::P_ResetMovementSpeed() { SetMaxWalkSpeed(EMoveMode::Walk); }

void AEnemy::P_ReceiveStunEffect(const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (AnimInstance)
	{
		AnimInstance->ClearMontageWithHoldHandle();
		AnimInstance->StopAllMontages(0.f);
	}
	if (GetWeapon()->GetAnimInstance()) GetWeapon()->GetAnimInstance()->StopAllMontages(0.f);
	InHitDirection.Z = 0.f;
	GetCharacterMovement()->StopMovementImmediately();
	ReceiveDamage(InDamage, EDamageReason::None, nullptr, nullptr);
	LaunchCharacter(InHitDirection * (P_Data ? P_Data->GetBashImpulseForce() : 5000.f), true, true);
	ApplyStunEffect(InEffectDuration);
}

void AEnemy::P_StunEffectEnded()
{
	SetInputState(false);
	P_SetLookLocked(false);
}

void AEnemy::P_MakeCameraShake(const ECameraShake InCameraShakeName)
{
	switch (InCameraShakeName)
	{
		case ECameraShake::WardenBeam: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(WardenBeamShake); break;
		case ECameraShake::MinionBossStomp: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(MinionBossStompCameraShake); break;
		case ECameraShake::MinionBossLaser: MinionBossLaserShakeInstance = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(MinionBossLaserShake); break;
		case ECameraShake::PlayerReceiveDamage: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(ReceivingDamageCameraShake); break;
		case ECameraShake::PlayerQuickMelee: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(QuickMeleeCameraShake); break;
		case ECameraShake::PlayerStomp: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(StompCameraShake); break;
		case ECameraShake::PlayerStunned: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(StunnedCameraShake); break;
		case ECameraShake::WraithMeteorLandCameraShake: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(WraithMeteorLandCameraShake); break;
		case ECameraShake::AcoDashInitiationCameraShake: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(AcoDashInitiationCameraShake); break;
		case ECameraShake::AcoPlatformRiseCameraShake: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(AcoRisePlatformCameraShake); break;
		case ECameraShake::AcoSlamCameraShake: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(AcoSlamCameraShake); break;
		case ECameraShake::PyroBossStompCameraShake1: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(PyroBossStompCameraShake1); break;
		case ECameraShake::PyroBossStompCameraShake2: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(PyroBossStompCameraShake2); break;
		case ECameraShake::SoldierBossMissileImpactCameraShake: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(SoldierBossMissileImpactCameraShake); break;
		default: ;
	}
}

void AEnemy::SetIsActionInMotion(const bool bState)
{
	bIsActionInMotion = bState;
	if (!AI) return;
	AI->GetStateTreeData().SetIsActionInMotion(bState);
	if (!bState) AI_SetupShootLogic();
}

void AEnemy::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	ConductionLogicData.TickStackDuration(DeltaTime);
	if (!bIsAlive || bInUpgradeSelection || Abilities.IsEmpty()) return;
	TickAbilities(DeltaTime);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	SetupUserSettings();
	if (const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller))
	{
		if (const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (!Subsystem->HasMappingContext(PlayerMappingContext)) Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
	EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(0).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Move); // WASD
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(4).Action, ETriggerEvent::Started, this, &AEnemy::P_Jump); // Space
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(5).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Rotate); // Mouse movement
		P_SetupShiftInput(); // Shift
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(7).Action, ETriggerEvent::Triggered, this, &AEnemy::P_HijackEnemy); // E
		P_SetupLeftClickInput(); // Left Click
		if (bCanReload) EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(9).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Reload); // R
		#if WITH_EDITOR
			EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(10).Action, ETriggerEvent::Started, this, &AEnemy::P_ToggleGodMode); // +
		#endif
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(11).Action, ETriggerEvent::Started, this, &AEnemy::P_UseAbility, P_GetFirstSlotAbilityIndex()); // 1
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(12).Action, ETriggerEvent::Started, this, &AEnemy::P_UseAbility, P_GetSecondSlotAbilityIndex()); // 2
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(13).Action, ETriggerEvent::Started, this, &AEnemy::P_OpenUpgradeSelection); // T
		P_SetupRightClickInput(); // Right Click
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &AEnemy::P_HandleEscapeClick); // Escape
		#if WITH_EDITOR
			EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(16).Action, ETriggerEvent::Started, this, &AEnemy::ToggleTimeDilation); // B
		#endif
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(17).Action, ETriggerEvent::Started, this, &AEnemy::P_TryInteracting); // F
		P_BindQuickMeleeAction(); // V
		//- 19 reserved was stomp.
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(20).Action, ETriggerEvent::Started, this, &AEnemy::OpenInfoPanel); // Tab
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(20).Action, ETriggerEvent::Completed, this, &AEnemy::CloseInfoPanel); // Tab
		P_BindSlideAction(); // C
	}
	if (!SaveSlotManager->GetSaveSlot()->GetIsTutorialFinished()) SetupTutorialManager();
}

void AEnemy::PossessedBy(AController* NewController)
{
	if (bIsHijacked) return;
	Super::PossessedBy(NewController);
	if (!Cast<APlayerController>(NewController)) return;
	DelegateManager->OnTimeDilationChanged.RemoveAll(this);
	bIsInvincible = true;
	GetWorldTimerManager().SetTimer(HijackInvincibilityDelayHandle, this, &AEnemy::P_TurnOffInvincibility, InvincibilityDelay);
	DisableAllVfxAfterPossession();
	bIsActionInMotion = false;
	P_TriggerGlitchingEffect();
	P_ToggleCriticallyDamagedEffects(0.f);
	AssetsManager->PlaySound2D(ESound::OnHijack);
	AssetsManager->RemoveStoredSound(ESound::GuardianSlowMotion);
	AssetsManager->RemoveStoredSound(ESound::CriticalDamageBeepFps);
	AssetsManager->RemoveStoredSound(ESound::PlayerBurning);
	GetWorldTimerManager().SetTimer(AI_TriggerDeathVoiceLineHandle, this, &AEnemy::AI_TriggerDeathVoiceLine, 1.f);
	P_Setup();
	DelegateManager->RequirePlayerAnnouncementInfo.ExecuteIfBound(NSLOCTEXT("ST_Localization", "Hijacked", "Hijacked"));
	DelegateManager->OnHijack.Broadcast(this);
	SetInputState(false);
}

void AEnemy::InitiateDeath()
{
	DisableVfxs();
	GetMesh()->SetOverlayMaterial(nullptr);
	ClearTimers();
	bIsAlive = false;
	Tags.Remove("AliveEnemy");
	if (!bIsHijacked)
	{
		if (bIsAco) (void)DelegateManager->OnAcoEnemyDeath.ExecuteIfBound();
		if (CurrentHp > 0.f && IsHackTerminalsGameMode()) (void)DelegateManager->HackTerminalSubtractHp.ExecuteIfBound(CurrentHp);
		AI_PlayDeathSound();
		AI_PlayDeathExplosion();
		AI_Destroy();
		if (AI_Data) (void)DelegateManager->OnEnemyDeath.ExecuteIfBound(AI_Data->GetXp(), CharacterName);
		DelegateManager->OnEnemyDeathOccurred.Broadcast(this);
		(void)DelegateManager->UpdateGameModeEnemiesAmount.ExecuteIfBound();
		if (bAffectedByEmp && DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::TricksterAbility2, EUpgradeAttribute::Special1)) AI_TriggerEmpExplosion();
	}
	else P_Destroy();
	AI_PlayDeathElectrification();
	HandleDeactivation();
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AEnemy::DestroyThis, DestroyAfter, false);
}

void AEnemy::ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent)
{
	if (bIsSelectedFourthOptionB) InDamage *= GameInstance->GetSubsystem<UPlayerDataManager>()->GetRecentHijackBoost();
	InDamage += InDamage * ConductionLogicData.GetDamageTakenPercentage();
	CurrentHp -= InDamage;
	if (IsHackTerminalsGameMode()) (void)DelegateManager->HackTerminalSubtractHp.ExecuteIfBound(InDamage + (CurrentHp <= 0.f ? CurrentHp : 0.f));
	if (bIsHijacked)
	{
		switch (InDamageReason)
		{
			case EDamageReason::GuardianDamage:
				if (EVariation::GuardianVariationGold == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ECharacterType::Guardian)) { ConductionLogicData.AddStack(); }
				break;
			case EDamageReason::WraithEnemyMeteor:
				if (DelegateManager->RequestSpecialUpgradeAvailability.IsBound() && DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::WraithAbility3, EUpgradeAttribute::Special1))
				{
					SetupBurningState(EBurningType::Meteor, 1.f, 3.f, 0.2f);
				}
				break;
			default: ;
		}
		P_HandlingDamageTaken();
		P_ShowReceivedDamageNumber(InDamage);
	}
	else if (bIsAlive)
	{
		if (RadialDamageEvent)
		{
			(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
			(void)DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(false);
		}
		else if (PointDamageEvent)
		{
			const bool bIsCritical = PointDamageEvent->HitInfo.BoneName.IsEqual(FName("head"));
			(void)DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(bIsCritical);
			(void)DelegateManager->ShowDamageNumber.ExecuteIfBound(bIsCritical ? CriticalDamageNumberColor : GetDamageNumberColor(), InDamage, PointDamageEvent->HitInfo.Location, GetActorForwardVector());
		}
		else
		{
			(void)DelegateManager->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
			(void)DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(false);
		}
		AI_ReceivedDamage(InDamageReason, InDamage);
	}
	if (bIsAlive && CurrentHp <= 0)
	{
		if (bIsHijacked)
		{
			P_Death();
			InitiateDeath();
		}
		else
		{
			ProcessKillDamageReason(InDamageReason);
			InitiateDeath();
			if (DelegateManager->RequestSpecialUpgradeAvailability.IsBound() && DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::WraithAbility1, EUpgradeAttribute::Special1) && BurningLogicData.GetIsBurning())
			{
				PlaySound(ESound::RangerGrenadeExplosion);
				AssetsManager->SpawnVfx(EVfx::RangerGrenadeExplosion, nullptr, GetActorLocation());
				const float TrueMolotovExplosionBaseDamage = MolotovExplosionBaseDamage * BurningLogicData.GetSpecialExplosionDamageBoostPercentage();
				UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), TrueMolotovExplosionBaseDamage, TrueMolotovExplosionBaseDamage,
				GetActorLocation(), MolotovExplosionRadius, MolotovExplosionRadius, 0.f, UDamageType::StaticClass(),
				TArray<AActor*>{this}, this, GetController(), ECC_GameTraceChannel4);
			}
		}
	}
}

void AEnemy::PlaySound(const ESound InSoundName, const bool bIsCallbackRequired)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() :
	AssetsManager->ConfigureSound(Sounds[InSoundName], bIsCallbackRequired ? [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); } : TFunction<bool()>(), InSoundName, bIsHijacked);
}

void AEnemy::StopSound(const ESound InSoundName)
{
	if (!Sounds.Contains(InSoundName)) return;
	Sounds[InSoundName]->Stop();
}

void AEnemy::EmpEffectEnded()
{
	bAffectedByEmp = false;
	RemoveDisablingActionEffect();
}

void AEnemy::ChangeOverlappingPoisonAreasInState(const bool bState)
{
	if (bState)
	{
		if (++OverlappingPoisonAreas == 1)
		{
			PoisonActionSpeedReduction = ActionSpeed - ActionSpeed / (bIsHijacked ? 2.f : 3.f);
			SetActionSpeed(ActionSpeed - PoisonActionSpeedReduction);
			if (!bIsHijacked) TogglePoisonSlowVfx(true);
		}
	}
	else
	{
		if (OverlappingPoisonAreas > 0 && --OverlappingPoisonAreas == 0)
		{
			SetActionSpeed(ActionSpeed + PoisonActionSpeedReduction);
			PoisonActionSpeedReduction = 0.f;
			if (!bIsHijacked) TogglePoisonSlowVfx(false);
		}
	}
}

void AEnemy::SetMovementSpeedMultiplier(const float InMultiplier)
{
	MovementSpeedMultiplier = InMultiplier;
	SetMaxWalkSpeed(CurrentMoveMode);
}

void AEnemy::RemoveDisablingActionEffect()
{
	DisablingActionsEffectsCount--;
	DisablingActionsEffectsCount = FMath::Max(0, DisablingActionsEffectsCount);
	if (DisablingActionsEffectsCount == 0) AllDisablingActionsFinished();
}

void AEnemy::SetupBurningState(const EBurningType InBurningType, const float InBurningDamage, const float InBurningDuration, const float InBurningTick, const float InBurningDamageBoostPercentage)
{
	if (BurningLogicData.GetBurningType() != InBurningType)
	{
		if (bIsHijacked) P_AddAndSetBurnVfx();
		GetWorldTimerManager().SetTimer(BurningTickingHandle, this, &AEnemy::ReceiveBurnDamage, InBurningTick, true);
	}
	BurningLogicData.AddBurnState(InBurningType, InBurningDamage, InBurningTick, InBurningDamageBoostPercentage);
	GetWorldTimerManager().SetTimer(BurningStateFinishHandle, this, &AEnemy::StopBurningState, InBurningDuration);
}

void AEnemy::AddSlowingStack(const float InSlowPercentage)
{
	if (WraithTrapSlowEffectLingerHandle.IsValid()) GetWorldTimerManager().ClearTimer(WraithTrapSlowEffectLingerHandle);
	SlowingStacks++;
	WraithTrapActionSpeedReduction = 1.f - FMath::Pow(1.f - InSlowPercentage, SlowingStacks);
	ActionSpeed = FMath::Pow(1.f - InSlowPercentage, SlowingStacks);
	SetActionSpeed(ActionSpeed);
}

void AEnemy::RemoveSlowingStacks()
{
	SlowingStacks = 0;
	ActionSpeed += WraithTrapActionSpeedReduction;
	SetActionSpeed(ActionSpeed);
}

void AEnemy::RemoveActiveWraithTrap(const float InEffectDuration)
{
	if (DelegateManager->RequestSpecialUpgradeAvailability.IsBound() && DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::WraithAbility2, EUpgradeAttribute::Special1))
	{
		GetWorldTimerManager().SetTimer(WraithTrapSlowEffectLingerHandle, this, &AEnemy::RemoveSlowingStacks, InEffectDuration);
	}
	else if (--ActiveWraithTrapCount == 0) RemoveSlowingStacks();
}

void AEnemy::Pull(const FVector& InPullLocation, const float InPullStrength)
{
	const FVector CurrentLocation = GetActorLocation();
	FVector Direction = InPullLocation - CurrentLocation;
	Direction.Z = 0.f;
	if (Direction.Size() <= KINDA_SMALL_NUMBER) return;
	Direction.Normalize();
	AddMovementInput(Direction, InPullStrength);
}

void AEnemy::ApplyTankShieldActionSpeedDebuff()
{
			UE_LOG(LogTemp, Error, TEXT("DEBUFF ME: %s"), *GetName());
	bTankShieldActionSpeedDebuff = true;
	SetActionSpeed(ActionSpeed);
	GetWorldTimerManager().SetTimer(AI_TankShieldActionSpeedDebuffHandle, this, &AEnemy::AI_TankShieldActionSpeedDebuffEnded, TankShieldActionSpeedDebuffDuration);
}

float AEnemy::AI_GetShootPause() { return AI_Data->GetShootPause(); }

void AEnemy::AI_SetupAbilities()
{
	const TMap<EAbility, TSoftObjectPtr<UAbilityData>> AbilitiesData = AssetsManager->GetCharacterData(CharacterName).GetCharacterContainer()->AI_Abilities_Data;
	const TObjectPtr<UAbilityManager> AbilityManager = GameInstance->GetSubsystem<UAbilityManager>();
	for (const TPair<EAbility, TSoftObjectPtr<UAbilityData>> AbilityData : AbilitiesData)
	{
		const TObjectPtr<UAbilityData> LoadedAbilityData = AbilityData.Value.LoadSynchronous();
		if (TObjectPtr<UAbility> Ability = AbilityManager->GetAbility(this, CharacterName, LoadedAbilityData->GetAbilityIndex()))
		{
			Ability->Setup(this, LoadedAbilityData, DelegateManager);
			Abilities.Add(LoadedAbilityData->GetAbilityIndex(), Ability);
		}
	}
}

void AEnemy::AI_Setup()
{
	AI_LoadDataIfNotLoaded(CharacterName);
	AI_SetController();
	AI->SetProprietor(this);
	AI_SetupWeapon();
	AI->SetupAI();
	AI_SetupAbilities();
	AI_SetupData();
	if (Weapon && Weapon->GetIsReloadEnabled()) AI->SetReloadDelay(AnimInstance->GetMontageLength(EEnemyAnimState::Reload));
	AI->GetStateTreeData().OnStateTreeEvent.BindUObject(this, &AEnemy::AI_FinishShootLogicByEvent);
	AI_SetupShootLogic();
}

void AEnemy::AI_SetupWeapon()
{
	if (!Weapon) return;
	Weapon->SetProprietor(this);
	AI->SetShootCooldown(Weapon->GetShootCooldown());
	Weapon->PostProprietorSetup();
}

void AEnemy::AI_Shoot() { Weapon->AI_Shoot(); }

void AEnemy::AI_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName)
{
	if (!AI_Data) AI_Data = AssetsManager->GetCharacterData(PrimaryDataAssetName).GetCharacterContainer()->AI_Data.LoadSynchronous();
}

void AEnemy::AI_SetController() { if (GetController()) AI = Cast<AEnemyAIController>(GetController()); }

void AEnemy::AI_PlayShootMontage()
{
	if (!bShootPauseActive && AnimInstance->IsMontageExisting(EEnemyAnimState::Shoot) && !AnimInstance->IsPlaying(EEnemyAnimState::Shoot))
	{
		AnimInstance->PlayMontage(EEnemyAnimState::Shoot);
		AI_SetCanShoot(true);
	}
}

void AEnemy::AI_Reload() const
{
	AnimInstance->PlayMontage(EEnemyAnimState::Reload);
	if (Weapon->GetAnimInstance() && Weapon->GetAnimInstance()->IsMontageExisting(EWeaponAnimState::AIReload))
	{
		Weapon->GetMesh()->SetComponentTickEnabled(true);
		Weapon->GetAnimInstance()->PlayMontage(EWeaponAnimState::AIReload);
	}
	Weapon->AI_ResetShootCycles();
}

void AEnemy::AI_PlayDeathSound() { PlaySound(ESound::MechanicalDeath); }

void AEnemy::AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage)
{
	if (MI_MeshBaseMaterial) MI_MeshBaseMaterial->SetScalarParameterValue("RedFlash", 1.f);
	FTimerHandle AI_DamageTakenEffectHandle;
	GetWorldTimerManager().SetTimer(AI_DamageTakenEffectHandle, this, &AEnemy::AI_DisableDamageTakenRedFlash, 0.02f);
	ProcessDamageReason(InDamageReason, InDamage);
	if (CurrentHp <= 0.f || !AI_HpBar || !AI_HpBarComponent) return;
	AI_ShowHpBar();
	AI_HpBar->UpdateHpBar(CurrentHp, MaxHp);
	bHasBeenDamagedRecently = true;
	GetWorldTimerManager().SetTimer(AI_HideHpBarHandle, this, &AEnemy::AI_HideHpBarByTimer, 2.f, false);
}

void AEnemy::AI_Destroy()
{
	AI_DestroyHpBar();
	AI_DestroyVfxs();
	if (AI)
	{
		GetWorld()->GetSubsystem<UEnemyCommunicator>()->RemoveEnemy(this);
		DelegateManager->OnXrayChange.RemoveAll(this);
		AI_StopAllAbilities();
		GetMesh()->SetAnimInstanceClass(nullptr);
		GetWorld()->GetTimerManager().ClearTimer(AI_ShootLogicHandle);
		AI->DestroyThis();
		AI->UnPossess();
		AI->Destroy();
		AI = nullptr;
	}
}

void AEnemy::AI_DestroyVfxs()
{
	if (EmpAffectedVfx)
	{
		EmpAffectedVfx->DeactivateImmediate();
		EmpAffectedVfx->DestroyComponent();
		EmpAffectedVfx = nullptr;
	}
	if (PoisonSlowAffectedVfx)
	{
		PoisonSlowAffectedVfx->DeactivateImmediate();
		PoisonSlowAffectedVfx->DestroyComponent();
		PoisonSlowAffectedVfx = nullptr;
	}
	if (StunAffectedVfx)
	{
		StunAffectedVfx->DeactivateImmediate();
		StunAffectedVfx->DestroyComponent();
		StunAffectedVfx = nullptr;
	}
	if (HijackableVfx)
	{
		HijackableVfx->DeactivateImmediate();
		HijackableVfx->DestroyComponent();
		HijackableVfx = nullptr;
	}
	if (SpySnatchMarkerVfx)
	{
		SpySnatchMarkerVfx->DeactivateImmediate();
		SpySnatchMarkerVfx->DestroyComponent();
		SpySnatchMarkerVfx = nullptr;
	}
	if (CorruptedVfx)
	{
		CorruptedVfx->DeactivateImmediate();
		CorruptedVfx->DestroyComponent();
		CorruptedVfx = nullptr;
	}
}

void AEnemy::AI_DisableActions(const float InEffectDuration, const EDisablingEffect InEffectName)
{
	GetWorld()->GetTimerManager().ClearTimer(AI_ShootLogicHandle);
	AI_StopAbilitiesEvents();
	if (AnimInstance && DisablingActionsEffectsCount < 2)
	{
		AnimInstance->ClearMontageWithHoldHandle();
		AnimInstance->StopAllMontages(0.f);
	}
	if (GetWeapon()->GetAnimInstance()) GetWeapon()->GetAnimInstance()->StopAllMontages(0.f);
	switch (InEffectName)
	{
		case EDisablingEffect::Emp: ApplyEmpEffect(InEffectDuration); break;
		case EDisablingEffect::Stun: ApplyStunEffect(InEffectDuration);
	}
}

float AEnemy::P_GetMeleeDamage()
{
	const TObjectPtr<USaveSlot> SaveSlot = SaveSlotManager->GetSaveSlot();
	const bool bIsUpgradeSelected = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FifthOptionA);
	return QuickMeleeAttackDamage + (bIsUpgradeSelected ? QuickMeleeAttackDamage * SaveSlot->GetPlayerLevel() * 0.15f : 0.f);
}

float AEnemy::P_GetStompDamage()
{
	const TObjectPtr<USaveSlot> SaveSlot = SaveSlotManager->GetSaveSlot();
	const bool bIsUpgradeSelected = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FifthOptionA);
	return StompDamage + (bIsUpgradeSelected ? StompDamage * SaveSlot->GetPlayerLevel() * 0.15f : 0.f);
}

float AEnemy::P_GetStompSize() { return StompRadiusInMeters * 100.f; }

void AEnemy::P_SetupAbilities()
{
	TArray<int32> ChosenAbilities = SaveSlotManager->GetSaveSlot()->GetCharacterChosenAbilities(CharacterName).Abilities;
	const TMap<EAbility, TSoftObjectPtr<UAbilityData>> AbilityData = AssetsManager->GetCharacterData(CharacterName).GetCharacterContainer()->P_Abilities_Data;
	const TObjectPtr<UAbilityManager> AbilityManager = GameInstance->GetSubsystem<UAbilityManager>();
	for (int32 i = 0; i < ChosenAbilities.Num(); i++)
	{
		if (TObjectPtr<UAbility> Ability = AbilityManager->GetAbility(this, CharacterName, ChosenAbilities[i]))
		{
			if (P_HUD) P_HUD->SetAbilityIcon(i, ChosenAbilities[i]);
			Ability->Setup(this, AbilityData[AbilityManager->GetAbilityName(CharacterName, ChosenAbilities[i])].LoadSynchronous(), DelegateManager);
			Ability->SetAbilityIndex(i);
			Abilities.Add(ChosenAbilities[i], Ability);
			if (i == 0) P_FirstSlotAbilityIndex = ChosenAbilities[i];
			else P_SecondSlotAbilityIndex = ChosenAbilities[i];
		}
	}
}

void AEnemy::P_Setup()
{
	P_RemoveWeakSpots();
	ResetActionSpeed();
	if (bAffectedByEmp) EmpEffectEnded();
	if (bAffectedByStun) AI_StunEffectEnded();
	ConductionLogicData.Reset();
	const float HijackSpeedMultiplier = SaveSlotManager->GetSaveSlot()->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::SecondOptionA) ? 1.3f : 1.f;
	HijackDelayAnimationSpeed = HijackSpeedMultiplier;
	HijackDelay /= HijackSpeedMultiplier;
	DisableSkeletalMesh();
	GetCapsuleComponent()->SetCollisionProfileName("Player");
	GetCharacterMovement()->SetComponentTickInterval(1.f / 60.f);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	DelegateManager->RequestCurrentCharacter.BindUObject(this, &AEnemy::GetCharacterName);
	DelegateManager->ApplyUpgradeOnSelection.BindUObject(this, &AEnemy::P_ApplyUpgradeOnSelection);
	DelegateManager->OnUpgradeSelectionOpened.BindUObject(this, &AEnemy::P_InUpgradeSelection);
	DelegateManager->RequestTriggerCameraShake.BindUObject(this, &AEnemy::P_MakeCameraShake);
	DelegateManager->RequestStopCameraShake.BindUObject(this, &AEnemy::P_StopCameraShake);
	DelegateManager->OnVariationSelectionEnd.BindUObject(this, &AEnemy::P_OnVariationSelection);
	DelegateManager->OnAbilitiesOrLevelMenuSelectionEnd.BindUObject(this, &AEnemy::P_OnAbilitiesOrLevelMenuSelectionEnd);
	DelegateManager->OnLevelUp.AddUObject(this, &AEnemy::P_PlayLevelUpVfx);
	DelegateManager->OnXpReceived.BindUObject(this, &AEnemy::P_AddXp);
	DelegateManager->ApplySecretBoostHealth.BindUObject(this, &AEnemy::P_ApplySecretBoostHp);
	DelegateManager->AddActionSpeedBoost.RemoveAll(this);
	DelegateManager->OnRunModeChange.BindUObject(this, &AEnemy::P_SetupRunMode);
	DelegateManager->RequestPlayerHp.BindUObject(this, &AEnemy::GetCurrentHp);
	Tags.Remove("AliveEnemy");
	Tags.Add("Player");
	P_ReceiveCurrentlySelectedVariation();
	AI_Destroy();
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->SetTarget(this);
	P_LoadDataIfNotLoaded(CharacterName);
	bUseControllerRotationYaw = true;
	SetupStimuliSource();
	//P_SetupNavArea();
	P_SetupData();
	P_SetupCharacterMovement();
	Camera->SetActive(true);
	GetWorldTimerManager().SetTimer(P_ControlLogicHandle, this, &AEnemy::P_ControlLogic, 0.05f, true);
	Weapon->P_SetupTransfer();
	GetMesh()->SetHiddenInGame(true);
	P_InputManager = Cast<UEnhancedPlayerInput>(GetWorld()->GetFirstPlayerController()->PlayerInput);
	P_SetupUI();
	P_SetupAbilities();
	DelegateManager->OnEnemyDeath.BindUObject(this, &AEnemy::P_OnEnemyDeathEvents);
	GetWorldTimerManager().SetTimer(P_CheckStoppedMovingHandle, this, &AEnemy::P_CheckStoppedMoving, 0.25f, true);
}

void AEnemy::P_Shoot()
{
	if (bIsInputLocked || bIsActionInMotion || !bCanShoot) return;
	if (Weapon->P_CanShoot())
	{
		if (bIsRunning)
		{
			if (bPlayerRunModeIsToggle) bWasRunningBeforeShooting = true;
			P_ToggleRun(false);
		}
		if (bIsSliding)
		{
			Weapon->GetHandsAnimInstance()->StopMontage(EHandsAnimState::Slide, 0.f);
			Weapon->GetHandsAnimInstance()->StopMontage(EHandsAnimState::SlideSecondary, 0.f);
		}
		P_WeaponShoot();
		return;
	}
	if (Weapon->GetCurrentAmmoAmount() > 0) return;
	if (Weapon->P_CanReload()) Weapon->P_Reload();
	else if (!Weapon->GetHandsAnimInstance()->IsPlaying(EHandsAnimState::Reload) && P_HUD) P_HUD->ShowOutOfAmmo();
}

void AEnemy::P_SetupLeftClickInput()
{
	P_SetupLeftClickInputStartedState();
	P_SetupLeftClickInputTriggeredState();
	P_SetupLeftClickInputCompletedState();
}

void AEnemy::P_SetupShiftInput() { P_SetupRunMode(); }

void AEnemy::P_SetupLeftClickInputStartedState()
{
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &AEnemy::P_ChangeHoldingShootState, true);
}

void AEnemy::P_SetupLeftClickInputTriggeredState()
{
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Shoot);
}

void AEnemy::P_SetupLeftClickInputCompletedState()
{
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Completed, this, &AEnemy::P_ChangeHoldingShootState, false);
}

void AEnemy::P_Destroy()
{
	P_RemoveBurnVfx();
	P_RemoveReloadSound();
	DelegateManager->RequestTriggerCameraShake.Unbind();
	GetMesh()->SetHiddenInGame(false);
	StimuliSource->UnregisterFromPerceptionSystem();
	Weapon->SetToOriginalSettings();
	Weapon->DestroyThis();
	GetMesh()->SetCollisionProfileName("Enemy");
	//NavAreaModifier->Destroy();
	if (bPlayerDied)
	{
		SaveSlotManager->GetSaveSlot()->SaveSlot();
		SaveSlotManager->ResetRunSaveSlot();
		return;
	}
	P_HUD->Reset();
	P_HUD = nullptr;
}

void AEnemy::P_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName)
{
	if (!P_Data) P_Data = AssetsManager->GetCharacterData(PrimaryDataAssetName).GetCharacterContainer()->P_Data.LoadSynchronous();
}

void AEnemy::P_SetupCharacterMovement()
{
	GetCharacterMovement()->GravityScale = GravityScale;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->AirControl = AirControl;
	GetCharacterMovement()->MaxAcceleration = MaxAcceleration;
	GetCharacterMovement()->BrakingFrictionFactor = BrakingFrictionFactor;
	GetCharacterMovement()->bUseRVOAvoidance = false;
	JumpMaxCount = 50; // Unreal engine stupid "Next Frame Update" movement system on ACharacter forces me to do this.
}

void AEnemy::P_Move(const FInputActionValue& Value)
{
	if (bIsInputLocked || bIsSliding || !Controller) return;
	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void AEnemy::P_InitiateJump()
{
	if (bIsSliding) P_SetupSuperJump();
	else GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	Jump();
	CurrentJumpCount++;
	Weapon->GetHandsAnimInstance()->PlayMontage(bIsJumpPrimaryAnim ? EHandsAnimState::Jump : EHandsAnimState::JumpSecondary);
	bIsDoubleJumpOccured = CurrentJumpCount == 2;
	AssetsManager->PlaySound2D(bIsDoubleJumpOccured ? ESound::DoubleJump : ESound::Jump);
}

void AEnemy::P_InitiateStomp()
{
	bIsStomping = true;
	AssetsManager->PlaySound2D(ESound::PlayerStompDashDown);
	GetCharacterMovement()->AirControl = 0.f;
	FVector Velocity = GetCharacterMovement()->Velocity;
	Velocity.Z = 0.f;
	GetCharacterMovement()->Velocity = Velocity + FVector(0.f, 0.f, -3000.f);
}

void AEnemy::P_ShowReceivedDamageNumber(const float InDamage) { (void)DelegateManager->OnPlayerDamageTaken.ExecuteIfBound(InDamage, FLinearColor::Red); }

void AEnemy::P_MakeCameraShake(const TSubclassOf<UCameraShakeBase>& InCameraShake) const { UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(InCameraShake); }

void AEnemy::P_StopAbility(const int32 InAbilityIndex)
{
	if (const TObjectPtr<UAbility>* Ability = Abilities.Find(InAbilityIndex)) (*Ability)->P_StopAbility();
}

void AEnemy::P_ProcessMeleeAttack()
{
	(void)DelegateManager->MeleeAttackOccured.ExecuteIfBound();
	FCollisionQueryParams OverlapParams;
	OverlapParams.AddIgnoredActor(this);
	TArray<FOverlapResult> Overlaps;
	const FVector Origin = GetActorLocation() - GetActorForwardVector() * 30.f;
	GetWorld()->OverlapMultiByProfile(Overlaps, Origin, FQuat::Identity, FName("TriggerDamageable"), FCollisionShape::MakeSphere(QuickMeleeAttackRadiusInMeters * 100.f), OverlapParams);
	for (const FOverlapResult& Overlap : Overlaps)
	{
		TObjectPtr<AActor> Actor = Overlap.GetActor();
		if (!Actor || (!Actor->ActorHasTag("AliveEnemy") && !Actor->ActorHasTag("Destructible"))) return;
		FVector ToTarget = (Actor->GetActorLocation() - Origin).GetSafeNormal();
		if (FVector::DotProduct(GetActorForwardVector(), ToTarget) < 0.342f) return;
		FHitResult Hit;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		TraceParams.AddIgnoredActor(Actor);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, Actor->GetActorLocation(), ECC_Visibility, TraceParams)) return;
		UGameplayStatics::ApplyDamage(Actor, P_GetMeleeDamage(), GetController(), this, UDamageType::StaticClass());
	}
}

void AEnemy::P_FinishHijackProcess()
{
	if (!HijackTarget || !HijackTarget->GetIsAlive())
	{
		P_CancelHijackingProcess();
		return;
	}
	GetWorldTimerManager().ClearTimer(HijackDelayHandle);
	GetWorldTimerManager().ClearTimer(HijackLockInToTargetHandle);
	P_StopAllAbilities(true);
	const FVector SpawnLocation = GetActorLocation();
	DelegateManager->OnEnemyDeath.Unbind();
	if (Controller) Controller->Possess(HijackTarget);
	InitiateDeath();
	AssetsManager->RequestSpawnEnemy(CharacterName, SpawnLocation, CurrentHp / MaxHp, true);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UHijackUI>(EWidget::HijackUI)->CloseUI();
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD)->PlayHijackInitiateAnim(false, HijackDelayAnimationSpeed);
	P_AddAndSetHijackPulse(0.f);
	DestroyImmediately();
}

void AEnemy::P_BindQuickMeleeAction() { EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(18).Action, ETriggerEvent::Started, this, &AEnemy::P_QuickMeleeAttack); }

void AEnemy::P_BindSlideAction() { EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(21).Action, ETriggerEvent::Started, this, &AEnemy::P_InitiateSlide); }

void AEnemy::P_StompImpact(const FVector& InHitLocation)
{
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, InHitLocation, FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(P_GetStompSize()));
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> Actor = Result.GetActor();
		if (!Actor || !Actor->ActorHasTag("AliveEnemy")) continue;
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(Actor);
		if (GetWorld()->LineTraceSingleByChannel(Hit, InHitLocation, Actor->GetActorLocation(), ECC_Visibility, Params)) continue;
		UGameplayStatics::ApplyDamage(Actor, P_GetStompDamage(), nullptr, this, UDamageType::StaticClass());
	}
}

void AEnemy::P_ToggleRun(const bool bState)
{
	if (bIsInputLocked || !bCanRun) return;
	bIsRunning = bState;
	if (bIsRunning)
	{
		if (!RunVfx) AssetsManager->AttachVfx(EVfx::RunEffect, [this]() -> bool { return GetIsAlive() && bIsRunning; }, &RunVfx, GetMesh(), FName("DeathSocket"));
		else RunVfx->Activate();
	}
	else if (RunVfx) RunVfx->DeactivateImmediate();
	SetMaxWalkSpeed(bIsRunning ? EMoveMode::Run : EMoveMode::Walk);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsInvincible) return 0.f;
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser) * DamageReduction;
	const FRadialDamageEvent* RadialDamageEvent = nullptr;
	const FPointDamageEvent* PointDamageEvent = nullptr;
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
	else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
	if (GetMesh()->IsSimulatingPhysics())
	{
		if (RadialDamageEvent) GetMesh()->AddRadialImpulse(RadialDamageEvent->Origin, RadialDamageEvent->Params.OuterRadius, 200000.f, RIF_Linear);
		else if (PointDamageEvent) GetMesh()->AddImpulseAtLocation(PointDamageEvent->ShotDirection * 10000.f, PointDamageEvent->HitInfo.Location);
		return 0.f;
	}
	if (bIsHijacked)
	{
		if (RadialDamageEvent)
		{
			FVector LaunchDir = GetActorLocation() - RadialDamageEvent->Origin;
			LaunchDir.Z += AnyExplosionUpwardsImpulseAxisAddition;
			LaunchDir.Normalize();
			LaunchCharacter(LaunchDir * AnyExplosionImpulseStrength, true, true);
		}
		if (EventInstigator != GetController()) ReceiveDamage(Damage, ReceiveDamageReason(DamageCauser), PointDamageEvent, RadialDamageEvent);
		return Damage;
	}
	ReceiveDamage(Damage, ReceiveDamageReason(DamageCauser), PointDamageEvent, RadialDamageEvent);
	return Damage;
}

float AEnemy::ReceiveDefaultMovementSpeed() const
{
	if (bIsHijacked) return GetMoveMode() == EMoveMode::Walk ? P_Data->GetMaxWalkSpeed() : P_Data->GetMaxRunSpeed();
	return GetMoveMode() == EMoveMode::Walk ? AI_Data->GetMaxWalkSpeed() : AI_Data->GetMaxRunSpeed();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UPossessionGameInstance>(GetWorld()->GetGameInstance());
	DelegateManager = GameInstance->GetSubsystem<UDelegateManager>();
	GameModeBase = Cast<APossessionGameGameModeBase>(GetWorld()->GetAuthGameMode());
	GlobalSettingsManager = GameInstance->GetSubsystem<UGlobalSettingsManager>();
	SaveSlotManager = GameInstance->GetSubsystem<USaveSlotManager>();
	if (Camera) DefaultCameraZ = Camera->GetRelativeLocation().Z;
	AI_SetupHpBar();
	AssetsManager = GameInstance->GetSubsystem<UPrimaryDataAssetsManager>();
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->AddEnemy(this);
	SetMesh(CharacterName);
	SetupMeshOptimization();
	if (WeakSpotMesh) AI_AddWeakSpots();
	SetAnimInstance();
	Tags.Add("Enemy");
	Tags.Add("AliveEnemy");
	GetWorldTimerManager().SetTimer(CheckIfFallOutOfWorldHandle, this, &AEnemy::CheckIfFallOutOfWorld, 5.f, true);
	GetWorldTimerManager().SetTimer(AI_SetCharacterMovementSpeedHandle, this, &AEnemy::AI_SetCharacterMovementSpeed, 0.05f, true);
	GetWorldTimerManager().SetTimer(AI_PlaySpawnSoundHandle, this, &AEnemy::AI_PlaySpawnSound, 3.f + UKismetMathLibrary::RandomInteger(4), false);
	ChangeXrayState(GlobalSettingsManager->GetIsXrayEnabled());
	DelegateManager->OnXrayChange.AddUObject(this, &AEnemy::ChangeXrayState);
	DelegateManager->AddActionSpeedBoost.AddUObject(this, &AEnemy::AddAdditionalActionSpeed);
	DelegateManager->OnTimeDilationChanged.AddUObject(this, &AEnemy::AI_TimeDilationChange);
	AI_Setup();
	SetInputState(false);
	GetWorldTimerManager().SetTimer(AI_RespawnTimerHandle, this, &AEnemy::AI_SetupRespawn, 0.05f);
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearTimers();
	if (AnimInstance) AnimInstance->ClearTimers();
	GetWorldTimerManager().ClearTimer(AI_ClearShootPauseHandle);
	GetWorldTimerManager().ClearTimer(AI_SetCharacterMovementSpeedHandle);
	GetWorldTimerManager().ClearTimer(AI_HideHpBarHandle);
	GetWorldTimerManager().ClearTimer(P_ContinuousFovChangeHandle);
	GetWorldTimerManager().ClearTimer(P_DeathHandle);
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	GetWorldTimerManager().ClearTimer(CheckIfFallOutOfWorldHandle);
	Super::EndPlay(EndPlayReason);
}

void AEnemy::SetMaxWalkSpeed(const EMoveMode InMoveMode, float InNewSpeed)
{
	SetMoveMode(InMoveMode);
	if (InNewSpeed < 0.f) InNewSpeed = ReceiveDefaultMovementSpeed();
	InNewSpeed *= ReceiveMovementSpeedModification();
	GetCharacterMovement()->MaxWalkSpeed = InNewSpeed * MovementSpeedMultiplier * ActionSpeed;
	GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.GetSafeNormal() * MovementSpeedMultiplier * InNewSpeed * ActionSpeed;
	if (AnimInstance) AnimInstance->SetMovementPlayRate(MovementSpeedMultiplier);
}

void AEnemy::AttachDestructible(const TObjectPtr<AActor>& Destructible, const FName SocketName) const
{
	Destructible->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void AEnemy::HandleDeactivation()
{
	if (Weapon) Weapon->DeactivateWeapon();
	GetCapsuleComponent()->DestroyComponent();
	if (AnimInstance) AnimInstance->ClearTimers();
	GetMesh()->SetAnimInstanceClass(nullptr);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetAllPhysicsLinearVelocity(ImpulseAfterDeath, false);
}

void AEnemy::SetMesh(const ECharacterType PrimaryDataAssetName) { GetMesh()->SetSkeletalMesh(AssetsManager->GetCharacterData(PrimaryDataAssetName).GetCharacterContainer()->CharacterMesh.Get()); }

void AEnemy::DestroyThis()
{
	if (bIsHijacked) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnLevelUp.RemoveAll(this);
	if (Weapon) Weapon->DestroyThis();
	GetWorldTimerManager().SetTimer(DestroyCondenseMaterialsProcessHandle, this, &AEnemy::DestroyCondenseMaterialsProcess, 0.05f, true);
}

void AEnemy::DestroyImmediately() { Destroy(); }

void AEnemy::DisableAllVfxAfterPossession()
{
	DisableHijackViableVfx();
	DisableSpySnatchMarkerVfx();
}

void AEnemy::ClearTimers()
{
	GetWorldTimerManager().ClearTimer(AI_TriggerDeathVoiceLineHandle);
	GetWorldTimerManager().ClearTimer(AI_PlaySpawnSoundHandle);
	GetWorldTimerManager().ClearTimer(P_ControlLogicHandle);
	GetWorldTimerManager().ClearTimer(P_DetectIfPlayerPointingAtOutlineObjectsHandle);
	GetWorldTimerManager().ClearTimer(P_TriggerCriticalDamageIndicatorHandle);
	GetWorldTimerManager().ClearTimer(P_SlideEndHandle);
	GetWorldTimerManager().ClearTimer(P_CheckStoppedMovingHandle);
	GetWorldTimerManager().ClearTimer(ActionSpeedHandle);
	GetWorldTimerManager().ClearTimer(BurningTickingHandle);
	GetWorldTimerManager().ClearTimer(BurningStateFinishHandle);
	GetWorldTimerManager().ClearTimer(DestroyCondenseMaterialsProcessHandle);
	GetWorldTimerManager().ClearTimer(SpawnCondenseMaterialsProcessHandle);
}

void AEnemy::AllDisablingActionsFinished()
{
	if (bIsHijacked || !AnimInstance) return;
	AnimInstance->PlayMontage(EEnemyAnimState::EmpRecover);
	GetWorldTimerManager().SetTimer(RecoverFromDisablingEffectHandle, this, &AEnemy::RecoverFromDisablingEffect, AnimInstance->GetMontageLength(EEnemyAnimState::EmpRecover));
}

void AEnemy::RecoverFromDisablingEffect()
{
	SetIsActionInMotion(false);
	if (AI) AI->ResumeLogic();
}

void AEnemy::ApplyEmpEffect(const float InEffectDuration)
{
	bAffectedByEmp = true;
	PlayEmpAffectedVfx(InEffectDuration);
	PlaySound(ESound::EmpEffectApplied, true);
	if (DisablingActionsEffectsCount < 2) AnimInstance->PlayMontageWithHold(EEnemyAnimState::EmpEffect, InEffectDuration, true);
	GetWorldTimerManager().SetTimer(EmpEffectEndHandle, this, &AEnemy::EmpEffectEnded, InEffectDuration);
}

void AEnemy::ReceiveBurnDamage() { UGameplayStatics::ApplyDamage(this, BurningLogicData.GetDamage(), nullptr, nullptr, UDamageType::StaticClass()); }

void AEnemy::Heal(const float InHealAmount)
{
	CurrentHp += InHealAmount;
	if (CurrentHp > MaxHp) CurrentHp = MaxHp;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(HealNumberColor, InHealAmount, GetActorLocation(), GetActorForwardVector());
	if (!AI_HpBar || !AI_HpBarComponent) return;
	AI_ShowHpBar();
	AI_HpBar->UpdateHpBar(CurrentHp, MaxHp);
	GetWorldTimerManager().SetTimer(AI_HideHpBarHandle, this, &AEnemy::AI_HideHpBarByTimer, 2.f, false);
}

void AEnemy::SetActionSpeed(const float InSpeed)
{
	if (!AI_Data || !AnimInstance) return;
	ActionSpeed = InSpeed + AdditionalActionSpeedValue;
	const float ModifiedActionSpeed = ActionSpeed * (bTankShieldActionSpeedDebuff ? TankShieldActionSpeedDebuffPercentage : 1.f);
	SetMaxWalkSpeed(CurrentMoveMode);
	AnimInstance->SetGlobalPlayRate(ModifiedActionSpeed);
	if (!Weapon) return;
	Weapon->SetReverseActionSpeed(1.f / ModifiedActionSpeed);
	if (Weapon->GetAnimInstance()) Weapon->GetAnimInstance()->SetGlobalPlayRate(ModifiedActionSpeed);
	else if (Weapon->GetHandsAnimInstance()) Weapon->GetHandsAnimInstance()->SetGlobalPlayRate(ModifiedActionSpeed);
}

void AEnemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	CurrentJumpCount = 0;
	bIsDoubleJumpOccured = false;
	if (!bIsStomping) return;
	bIsStomping = false;
	AssetsManager->PlaySound2D(ESound::PlayerStompLand);
	AssetsManager->SpawnVfx(EVfx::PlayerStomp, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, GetSocketLocation("RootSocket"));
	GetCharacterMovement()->AirControl = 1.f;
	P_StompImpact(GetSocketLocation("RootSocket"));
	P_MakeCameraShake(ECameraShake::PlayerStomp);
}

bool AEnemy::AI_IsSpySnatchVfxAvailable() const { return !bIsBoss && DelegateManager->RequestIsSpyMarkerRequired.IsBound() && DelegateManager->RequestIsSpyMarkerRequired.Execute(); }

void AEnemy::AI_SetupData()
{
	CurrentHp = MaxHp = AI_Data->GetMaxHp();
	bIsHijackAble = AI_Data->GetIsPossessable();
	SetMaxWalkSpeed(EMoveMode::Walk);
}

void AEnemy::AI_StopAllAbilities()
{
	for (const TPair<int32, TObjectPtr<UAbility>> Ability : Abilities) Ability.Value->AI_StopAbility();
	Abilities.Empty();
}

void AEnemy::AI_SetupHpBar()
{
	if (!AI_HpBarComponent) return;
	AI_HpBar = Cast<UAIHpBarUI>(AI_HpBarComponent->GetWidget());
	AI_HideHpBar();
}

void AEnemy::AI_DestroyHpBar()
{
	GetWorldTimerManager().ClearTimer(AI_HideHpBarHandle);
	if (!AI_HpBar || !AI_HpBarComponent) return;
	AI_HpBarComponent->DestroyComponent();
	AI_HpBar = nullptr;
	AI_HpBarComponent = nullptr;
}

void AEnemy::AI_TriggerEmpExplosion()
{
	AssetsManager->SpawnVfx(EVfx::EmpExplosion, nullptr, GetActorLocation());
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, GetActorLocation(), FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(ExplosionRadiusFromEmpUpgradeInMetres * 100.f));
	TSet<TObjectPtr<AActor>> UniqueActors;
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> HitActor = Result.GetActor();
		if (!HitActor || UniqueActors.Contains(HitActor)) continue;
		UniqueActors.Add(HitActor);
		if (HitActor->ActorHasTag("AliveEnemy")) UGameplayStatics::ApplyDamage(HitActor, AI_Data->GetMaxHp() * 0.25f, nullptr, this, UDamageType::StaticClass());
	}
}

void AEnemy::AI_HideHpBar() const
{
	if (!AI_HpBarComponent || bIsLookedAtByPlayer || bHasBeenDamagedRecently) return;
    AI_HpBarComponent->SetVisibility(false);
	AI_HpBarComponent->SetComponentTickEnabled(false);
}

void AEnemy::AI_HideHpBarByTimer()
{
	bHasBeenDamagedRecently = false;
	AI_HideHpBar();
}

void AEnemy::AI_ClearShootPause()
{
	bShootPauseActive = false;
	GetWorldTimerManager().ClearTimer(AI_ClearShootPauseHandle);
}

void AEnemy::AI_SetCharacterMovementSpeed() const { AnimInstance->SetSpeed(GetCharacterMovement()->Velocity.Size()); }

void AEnemy::AI_DisableDamageTakenRedFlash() const { if (MI_MeshBaseMaterial) MI_MeshBaseMaterial->SetScalarParameterValue("RedFlash", 0.f); }

void AEnemy::AI_FinishShootLogicByEvent(const EStateTreeEvent InEvent)
{
	if (InEvent == EStateTreeEvent::MinionBossMoveDirectlyToPlayer) return;
	GetWorld()->GetTimerManager().ClearTimer(AI_ShootLogicHandle);
	AI_CancelShooting();
}

void AEnemy::AI_StopAbilitiesEvents() const
{
	if (!AI) return;
	switch (AI->GetCurrentStateTreeEvent())
	{
		case EStateTreeEvent::WardenBeamShot: AI_StopAbility(0); break;
		case EStateTreeEvent::RangerLaunchGrenade: AI_StopAbility(0); break;
		case EStateTreeEvent::RangerDeployShield: AI_StopAbility(1); break;
		case EStateTreeEvent::TricksterLaunchPoisonGrenade: AI_StopAbility(0); break;
		case EStateTreeEvent::BreacherBash: AI_StopAbility(1); break;
		case EStateTreeEvent::WraithMeteorShower: AI_StopAbility(2); break;
		case EStateTreeEvent::MinionBossStomp: AI_StopAbility(0); break;
		case EStateTreeEvent::MinionBossLaserEye: AI_StopAbility(1); break;
		case EStateTreeEvent::PyroBossStomp: AI_StopAbility(1); break;
		case EStateTreeEvent::PyroBossBallPhase: AI_StopAbility(2); break;
		case EStateTreeEvent::SoldierBossMissilesLaunch: AI_StopAbility(0); break;
		default: ;
	}
}

void AEnemy::AI_TimeDilationChange(const float InDilation) const
{
	if (!bIsAlive) return;
	GetCharacterMovement()->SetComponentTickInterval(1.f / MovementFramesPerSecond / InDilation);
	GetMesh()->SetComponentTickInterval(1.f / 30.f / InDilation);
}

void AEnemy::AI_ShowHpBar() const
{
	if (!AI_HpBarComponent) return;
	AI_HpBarComponent->SetVisibility(true);
	AI_HpBarComponent->SetComponentTickEnabled(true);
}

void AEnemy::AI_SetSpySnatchMarker(const bool bState)
{
	if (!bState)
	{
		DisableSpySnatchMarkerVfx();
		return;
	}
	if (!AI_IsSpySnatchVfxAvailable()) return;
	if (!SpySnatchMarkerVfx)
	{
		if (AssetsManager->GetVfxData(EVfx::SpySnatchMarker).GetIsInLoad() && !AssetsManager->GetVfxData(EVfx::SpySnatchMarker).GetIsLoaded()) return;
		AssetsManager->AttachVfx(EVfx::SpySnatchMarker, [this]() -> bool { return GetIsAlive() && !bIsHijacked && bIsLookedAtByPlayer; }, &SpySnatchMarkerVfx, GetMesh(), FName("RootSocket"));
	}
	else if (!SpySnatchMarkerVfx->IsActive()) SpySnatchMarkerVfx->Activate();
}

void AEnemy::AI_ChangeWeakSpotsState(const bool bState)
{
	if (!bState)
	{
		for (int32 i = 0; i < WeakSpots.Num(); i++)
		{
			if (!WeakSpots[i]) continue;
			WeakSpots[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WeakSpots[i]->SetVisibility(false);
		}
		return;
	}
	TSet<int32> RandomIndices;
	while (RandomIndices.Num() < 4)
	{
		const int32 RandIndex = FMath::RandRange(0, WeakSpots.Num() - 1);
		if (!RandomIndices.Contains(RandIndex) && WeakSpots[RandIndex])
		{
			RandomIndices.Add(RandIndex);
			WeakSpots[RandIndex]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			WeakSpots[RandIndex]->SetVisibility(true);
		}
	}
}

void AEnemy::AI_AddWeakSpots()
{
	for (int32 i = 1; i <= WeakSpotsAmount; i++)
	{
		TObjectPtr<UStaticMeshComponent> WeakSpotMeshComponent = NewObject<UStaticMeshComponent>(this);
		WeakSpotMeshComponent->RegisterComponent();
		WeakSpotMeshComponent->SetStaticMesh(WeakSpotMesh);
		WeakSpotMeshComponent->SetComponentTickEnabled(false);
		FString SocketName = FString::Printf(TEXT("WeakSpotSocket%i"), i);
		WeakSpotMeshComponent->ComponentTags.Add("WeakSpot");
		WeakSpotMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(SocketName));
		WeakSpotMeshComponent->SetCollisionProfileName(FName("WeakSpot"));
		WeakSpotMeshComponent->SetVisibility(false);
		WeakSpotMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeakSpotMeshComponent->SetWorldScale3D(FVector(0.25f));
		WeakSpots.Add(WeakSpotMeshComponent);
	}
}

void AEnemy::AI_TankShieldActionSpeedDebuffEnded()
{
	bTankShieldActionSpeedDebuff = false;
	SetActionSpeed(ActionSpeed);
}

void AEnemy::AI_AcoMaterialChange() const
{
	if (!MI_MeshBaseMaterial) return;
	MI_MeshBaseMaterial->SetScalarParameterValue(FName("Saturation"), 0.f);
	MI_MeshBaseMaterial->SetScalarParameterValue(FName("AcoHijack"), 1.f);
}

void AEnemy::AI_MainShootLogic()
{
	if (!AI || !bAllowedToShoot || GetIsActionInMotion() || !AI->GetStateTreeData().GetIsTargetInMySight()) return;
	if (!GetAnimInstance()->IsPlaying(EEnemyAnimState::Shoot)) AI_InitiateShooting();
	if (GetWeapon()->AI_CanShoot()) AI_ShootLogic();
}

void AEnemy::AI_SetCombatState(const bool bState)
{
	bIsInvincible = !bState;
	bAllowedToShoot = bState;
	if (!AI) return;
	if (bState) AI->ResumeLogic();
	else AI->StopLogic(false);
}

void AEnemy::AI_DestroyBossUnnecessaryComponents()
{
	if (AI_HpBarComponent)
	{
		AI_HpBarComponent->DestroyComponent();
		AI_HpBarComponent = nullptr;
	}
	if (Camera)
	{
		Camera->DestroyComponent();
		Camera = nullptr;
	}
}

void AEnemy::P_ChangeHoldingShootState(const bool bState)
{
	bIsHoldingShoot = bState;
	if (!bIsHoldingShoot) P_ReEnableRunIfToggleMode();
}

void AEnemy::P_SetupData()
{
	P_Data->SetIsGodModeEnabled(GlobalSettingsManager->GetIsGodModeEnabled());
	const TObjectPtr<USaveSlot> SaveSlot = SaveSlotManager->GetSaveSlot();
	bIsSelectedFourthOptionB = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FourthOptionB);
	if (SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FourthOptionA)) GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddRecentHijackBoost(true);
	else if (bIsSelectedFourthOptionB) GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddRecentHijackBoost(false);
	const float HpPercentage = CurrentHp / (AI_Data->GetMaxHp() + SaveSlotManager->GetRunSaveSlot()->GetHpUpgradesForEnemy(CharacterName));
	MaxHp = P_Data->GetMaxHp();
	MaxHp += SaveSlot->GetPlayerLevelAllGlobalHpBonus(MaxHp);
	MaxHp += MaxHp * SaveSlotManager->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardHealth) * 0.01f;
	CurrentHp = MaxHp * HpPercentage;
	if (SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::SecondOptionB) && CurrentHp < MaxHp) CurrentHp = FMath::Min(CurrentHp + MaxHp * 0.25f, MaxHp);
	bIsHijacked = true;
	GetWorldTimerManager().ClearTimer(AI_PlaySpawnSoundHandle);
	bCanShoot = true;
	SetMaxWalkSpeed(EMoveMode::Walk);
}

void AEnemy::P_SetupUI()
{
	P_HUD = GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD);
	P_HUD->SetCharacterType(CharacterName);
	P_HUD->SetPlayerRef(this);
	P_HUD->UpdateProgressBar(EUIBar::Hijack, 0.f, P_Data->GetPossessionBarCooldown());
	P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
	const TObjectPtr<USaveSlot> SaveSlot = SaveSlotManager->GetSaveSlot();
	const TObjectPtr<URunSaveSlot> RunSaveSlot = SaveSlotManager->GetRunSaveSlot();
	P_HUD->UpdateProgressBar(EUIBar::Xp, SaveSlot->GetXp(), SaveSlot->GetCurrentLevelNeededXp());
	P_HUD->UpdateProgressBar(EUIBar::Upgrade, RunSaveSlot->GetCurrentUpgradeXp(), RunSaveSlot->GetCurrentUpgradeXpNeeded());
	P_HUD->ConfigureUIs();
	P_UpdateAbilitiesUI();
	Weapon->P_UpdateAmmoTextHUD();
	P_HUD->PlayHijackAnim(false, HijackDelayAnimationSpeed);
	if (CurrentHp <= MaxHp * 0.3f) P_EnableCriticalDamageIndicator();
	GetWorldTimerManager().SetTimer(P_DetectIfPlayerPointingAtOutlineObjectsHandle, this, &AEnemy::P_DetectIfPlayerPointingAtOutlineObjects, 0.015f, true);
}

void AEnemy::P_HijackEnemy()
{
	if (bIsInputLocked) return;
	if (bCanHijackOtherEnemy)
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		const FVector StartLocation = Camera->GetComponentLocation();
		QueryParams.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, StartLocation + Camera->GetForwardVector() * 15000.f, ECC_GameTraceChannel5, QueryParams))
		{
			P_HijackLogic(Cast<AEnemy>(HitResult.GetActor()), false);
		}
	}
}

void AEnemy::P_HijackLogic(const TObjectPtr<AEnemy>& InEnemy, const bool bInstantHijack)
{
	if (!InEnemy || InEnemy->bIsHijacked || !InEnemy->bIsAlive || HijackDelayHandle.IsValid()) return;
	if (!InEnemy->GetIsHijackAble())
	{
		if (P_HUD) P_HUD->ShowMinorAnnouncement(NSLOCTEXT("ST_Localization", "HijackUnavailable", "Hijack Unavailable"), ESound::NotSelectable);
		return;
	}
	if (bInstantHijack)
	{
		P_StopAllAbilities(true);
		DelegateManager->OnEnemyDeath.Unbind();
		if (Controller) Controller->Possess(InEnemy);
		InitiateDeath();
		DestroyImmediately();
	}
	else
	{
		P_AddAndSetHijackPulse(1.f);
		AssetsManager->PlaySound2D(ESound::HijackInProgress);
		HijackTarget = InEnemy;
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UHijackUI>(EWidget::HijackUI)->OpenUI(HijackDelayAnimationSpeed);
		SetInputState(true);
		P_SetLookLocked(true);
		if (P_HUD) P_HUD->PlayHijackAnim(true, HijackDelayAnimationSpeed);
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD)->PlayHijackInitiateAnim(true, HijackDelayAnimationSpeed);
		GetWorldTimerManager().SetTimer(HijackDelayHandle, this, &AEnemy::P_FinishHijackProcess, HijackDelay);
		GetWorldTimerManager().SetTimer(HijackFovChangeHandle, this, &AEnemy::P_ChangeFovWhileHijackingProcess, 0.02f, true);
		GetWorldTimerManager().SetTimer(HijackLockInToTargetHandle, this, &AEnemy::P_LockInToTargetLogic, 0.02f, true);
	}
}

void AEnemy::P_HijackBarLogic()
{
	HijackBarTimer += 0.05f;
	if (HijackBarTimer >= P_Data->GetPossessionBarCooldown())
	{
		bCanHijackOtherEnemy = true;
		if (P_HUD) P_HUD->UpdateHijackIndicator(true, GlobalSettingsManager->GetKeyBindText("Hijack"));
	}
	if (P_HUD) P_HUD->UpdateProgressBar(EUIBar::Hijack, HijackBarTimer, P_Data->GetPossessionBarCooldown());
}

void AEnemy::P_Rotate(const FInputActionValue& InValue)
{
	if (!Controller || bIsLookLocked) return;
	const FVector2D RotationVector = InValue.Get<FVector2D>();
	const FVector2D MouseSensitivity = GlobalSettingsManager->GetMouseSensitivity();
	AddControllerYawInput(RotationVector.X * MouseSensitivity.X);
	AddControllerPitchInput(-RotationVector.Y * MouseSensitivity.Y);
}

void AEnemy::P_Reload()
{
	if (bIsInputLocked) return;
	if (!bIsActionInMotion && Weapon->GetIsReloadEnabled() && bCanReload && !P_InputManager->IsPressed(FKey("LeftMouseButton")) && Weapon->P_CanReload()) Weapon->P_Reload();
}

void AEnemy::P_OnEnemyDeathEvents(float InXp, const ECharacterType InCharacterName) const
{
	if (InCharacterName != ECharacterType::Warden && InCharacterName != ECharacterType::MinionBoss && InCharacterName != ECharacterType::PyroBoss && InCharacterName != ECharacterType::SoldierBoss && InCharacterName != ECharacterType::AcoBoss && DelegateManager->RequestCurrentXpNerf.IsBound()) InXp *= DelegateManager->RequestCurrentXpNerf.Execute();
	P_AddXp(InXp);
}

void AEnemy::P_ToggleGodMode()
{
	GlobalSettingsManager->ToggleGodModeEnabled();
	P_Data->SetIsGodModeEnabled(GlobalSettingsManager->GetIsGodModeEnabled());
	MaxHp = P_Data->GetMaxHp();
	CurrentHp = MaxHp;
}

void AEnemy::P_UpdateAbilitiesUI()
{
	if (!bIsHijacked || !P_HUD) return;
	for (TPair<int32, TObjectPtr<UAbility>> Ability : Abilities) P_HUD->UpdateAbilityUI(Ability.Value);
}

void AEnemy::P_StopAllAbilities(const bool bDestroy)
{
	for (const TPair<int32, TObjectPtr<UAbility>> Ability : Abilities)
	{
		Ability.Value->P_StopAbility();
		if (bDestroy) Ability.Value->Destroy();
	}
}

void AEnemy::P_HandleEscapeClick()
{
	if (bIsInputLocked) return;
	switch (GetWorld()->GetSubsystem<UWidgetManager>()->GetCurrentStateOfUI())
	{
		case EWidget::PauseMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->CloseUI(); break;
		case EWidget::SettingsUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->CloseUI(); break;
		case EWidget::VariationsUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI)->CloseUI(); break;
		case EWidget::AbilitiesMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI)->CloseUI(); break;
		case EWidget::UpgradeSelectionUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->GoBack(); break;
		case EWidget::PlayerLevelSystemUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerLevelSystemUI>(EWidget::PlayerLevelSystemUI)->CloseUI(); break;
		case EWidget::ExplanationUI: break;
		default: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->OpenUI();
	}
}

void AEnemy::P_TriggerGlitchingEffect() const { GetWorld()->GetSubsystem<UPostProcessManager>()->TriggerGlitchingMaterial(1.25f); }

void AEnemy::P_StopCameraShake(const ECameraShake InCameraShakeName) const
{
	switch (InCameraShakeName)
	{
		case ECameraShake::MinionBossLaser: UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopCameraShake(MinionBossLaserShakeInstance); break;
		default: ;
	}
}

void AEnemy::P_HandlingDamageTaken()
{
	if (bIsImmortal && CurrentHp <= 10) CurrentHp = 10;
	P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
	#if WITH_EDITOR
		if (!GameInstance->IsPlayerTakenDamageEffectsEnabled()) return;
	#endif
	P_TriggerTakeDamageEffects();
	if (!bIsCriticallyDamaged && CurrentHp <= MaxHp * 0.3f) P_EnableCriticalDamageIndicator();
}

void AEnemy::P_TriggerTakeDamageEffects()
{
	if (!bPlayerTakeDamageEffectsEnabled) return;
	bPlayerTakeDamageEffectsEnabled = false;
	GetWorldTimerManager().SetTimer(P_TakeDamageEffectsHandle, this, &AEnemy::P_EnableTakeDamageEffects, PlayerTakeDamageEffectDelay);
	P_MakeCameraShake(ECameraShake::PlayerReceiveDamage);
	P_HUD->UpdateReceiveDamageInv();
	if (UKismetMathLibrary::RandomInteger(100) >= 75)
	{
		AssetsManager->PlaySound2D(ESound::TakeDamageLayer2);
		GetWorld()->GetSubsystem<UPostProcessManager>()->TriggerGlitchingMaterial(0.25f);
	}
	else AssetsManager->PlaySound2D(ESound::TakeDamageLayer1);
}

void AEnemy::P_EnableCriticalDamageIndicator()
{
	P_SetWarningIndicator(EWarningIndicator::CriticalDamage);
	GetWorldTimerManager().SetTimer(P_TriggerCriticalDamageIndicatorHandle, this, &AEnemy::P_TriggerCriticalDamageIndicator, 0.5f, true);
	bIsCriticallyDamaged = true;
	P_ToggleCriticallyDamagedEffects(1.f);
	AssetsManager->PlaySound2D(ESound::CriticalHealthEnter);
	AssetsManager->PlaySound2D(ESound::CriticalDamageBeepFps, true);
}

void AEnemy::P_ToggleCriticallyDamagedEffects(const float InValue) const
{
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::ScreenCrack, InValue);
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::BloodPumping, InValue);
}

void AEnemy::P_InitiateDeathEvents()
{
	if (P_HUD) P_HUD->PlayFadeOutAnim(true);
	GetWorldTimerManager().ClearTimer(P_DeathHandle);
	P_ConfigurePostProcessEffectsAfterDeath();
	AssetsManager->RemoveStoredSound(ESound::GuardianSlowMotion);
	AssetsManager->RemoveStoredSound(ESound::CriticalDamageBeepFps);
	AssetsManager->RemoveStoredSound(ESound::PlayerBurning);
	DelegateManager->OnPlayerDeath.Broadcast();
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, 3.f, FLinearColor::Black, false, true);
	if (!DelegateManager->RequestIsArenaMode.Execute()) GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UDeathScreenUI>(EWidget::DeathMenuUI)->OpenUI();
	else GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UDemoEndUI>(EWidget::DemoEndUI);
}

void AEnemy::P_ConfigurePostProcessEffectsAfterDeath() const
{
	const TObjectPtr<UPostProcessManager> PostProcessManager = GetWorld()->GetSubsystem<UPostProcessManager>();
	PostProcessManager->ToggleMaterial(EPostProcessMaterial::BloodPumping, 0.f);
	PostProcessManager->ToggleMaterial(EPostProcessMaterial::ScreenCrack, 0.f);
	PostProcessManager->ToggleMaterial(EPostProcessMaterial::DeathGlitching, 1.f);
	PostProcessManager->ToggleMaterial(EPostProcessMaterial::DeathBloodPumping, 1.f);
	PostProcessManager->ToggleMaterial(EPostProcessMaterial::DeathScreenCrack, 1.f);
	PostProcessManager->FadeOutDeathMaterials();
}

void AEnemy::P_InUpgradeSelection() { bInUpgradeSelection = true; }

void AEnemy::P_TryInteracting()
{
	if (bIsInputLocked) return;
	DelegateManager->OnInteractionTry.Broadcast();
}

void AEnemy::P_QuickMeleeAttack()
{
	if (bIsActionInMotion || bIsInputLocked) return;
	AssetsManager->PlaySound2D(ESound::QuickMelee);
	P_MakeCameraShake(ECameraShake::PlayerQuickMelee);
	bIsActionInMotion = true;
	GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::QuickMelee);
	P_ProcessMeleeAttack();
}

void AEnemy::P_ToggleRun() { P_ToggleRun(!bIsRunning); }

/*void AEnemy::P_SetupNavArea()
{
	NavAreaModifier = GetWorld()->SpawnActor<ANavAreaModifier>(P_NavAreaDefaultClass, GetActorLocation(), GetActorRotation());
	NavAreaModifier->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}*/

void AEnemy::P_OnVariationSelection(const ECharacterType InCharacterType)
{
	if (const TObjectPtr<AEnemy> Enemy = AssetsManager->SpawnVariationReplacement(InCharacterType, EWidget::VariationsUI)) P_HijackLogic(Enemy, true);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI)->CloseUI();
}

void AEnemy::P_OnAbilitiesOrLevelMenuSelectionEnd(const EWidget InWidgetName)
{
	P_HijackLogic(AssetsManager->SpawnVariationReplacement(CharacterName, InWidgetName), true);
}

void AEnemy::P_ReceiveCurrentlySelectedVariation() { Variation = GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(CharacterName); }

void AEnemy::P_Jump()
{
	if (bIsInputLocked || CurrentJumpCount >= MaxJumpAmount) return;
	P_InitiateJump();
}

void AEnemy::P_Stomp()
{
	if (bIsInputLocked || bIsStomping || !bCanStomp || !GetCharacterMovement()->IsFalling()) return;
	P_InitiateStomp();
}

void AEnemy::P_PlayLevelUpVfx()
{
	if (!LevelUpVfx) AssetsManager->AttachVfx(EVfx::LevelUpEffect, [this]() -> bool { return GetIsAlive(); }, &LevelUpVfx, GetMesh(), FName("DeathSocket"));
	else LevelUpVfx->Activate();
}

void AEnemy::P_Death()
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UHijackUI>(EWidget::HijackUI)->CloseUI();
	GetWorldTimerManager().ClearTimer(HijackDelayHandle);
	GetWorldTimerManager().ClearTimer(HijackLockInToTargetHandle);
	if (!bPlayerDied) AssetsManager->PlaySound2D(ESound::PlayerDeath);
	bPlayerDied = true;
	if (AssetsManager->IsAsyncLoadingFinished()) P_InitiateDeathEvents();
	else if (!bEnableUnloadLevel)
	{
		bEnableUnloadLevel = true;
		GetWorldTimerManager().SetTimer(P_DeathHandle, this, &AEnemy::P_Death, 0.01f, true);
	}
}

void AEnemy::P_ControlLogic()
{
	FVector Velocity = GetCharacterMovement()->Velocity;
	Velocity.Z = 0.f;
	Weapon->P_SetWalkSpread(Velocity.Size() >= P_Data->GetMaxWalkSpeed() - 0.1f);
	Weapon->P_SetWalkingSpeed(Velocity.Size());
	if (!bCanHijackOtherEnemy && !bInUpgradeSelection) P_HijackBarLogic();
}

void AEnemy::P_DetectIfPlayerPointingAtOutlineObjects()
{
	FHitResult HitResult;
	const FVector StartLocation = GetCamera()->GetComponentLocation();
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, StartLocation + GetCamera()->GetForwardVector() * 15000.f, ECC_Visibility))
	{
		const TObjectPtr<AActor> HitActor = HitResult.GetActor(); 
		if (HitActor->Implements<UInteractable>())
		{
			IInteractable* Interactable = Cast<IInteractable>(HitActor);
			if (Interactable && Interactable->GetIsInteractable()) Interactable->ChangeOutlineState(true);
		}
		else if (bCanHijackOtherEnemy)
		{
			const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(HitActor);
			if (Enemy && Enemy->GetIsAlive())
			{
				if (Enemy->GetIsHijackAble()) Enemy->AI_SetHijackOutline(true);
				else Enemy->AI_SetUnHijackOutline(true);
				return;
			}
		}
	}
}

void AEnemy::P_TriggerCriticalDamageIndicator() const { if (P_HUD) P_HUD->ToggleWarningIndicator(); }

void AEnemy::P_ContinuousFovChange()
{
	Camera->SetFieldOfView(Camera->FieldOfView + FovChangeTick);
	if ((FovChangeTick < 0.f && Camera->FieldOfView <= TargetFov) || (FovChangeTick > 0.f && Camera->FieldOfView >= TargetFov))
	{
		Camera->SetFieldOfView(TargetFov);
		GetWorldTimerManager().ClearTimer(P_ContinuousFovChangeHandle);
	}
}

void AEnemy::P_AddXp(const float InXp) const
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = SaveSlotManager->GetRunSaveSlot();
	const TObjectPtr<USaveSlot> SaveSlot = SaveSlotManager->GetSaveSlot();
	RunSaveSlot->AddUpgradeXp(GameInstance, InXp);
	SaveSlot->AddXp(InXp);
	if (!P_HUD) return;
	P_HUD->UpdateProgressBar(EUIBar::Xp, SaveSlot->GetXp(), SaveSlot->GetCurrentLevelNeededXp());
	P_HUD->UpdateProgressBar(EUIBar::Upgrade, RunSaveSlot->GetCurrentUpgradeXp(), RunSaveSlot->GetCurrentUpgradeXpNeeded());
}

void AEnemy::P_ApplySecretBoostHp(const int32 InPercentage)
{
	const int32 MaxHpIncrease = MaxHp * InPercentage * 0.01f;
	MaxHp += MaxHpIncrease;
	CurrentHp += MaxHpIncrease;
	if (P_HUD) P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
}

void AEnemy::P_InitiateSlide()
{
	if (bIsDoubleJumpOccured)
	{
		P_Stomp();
		return;
	}
	if (bIsSliding || !bCanSlide) return;
	AssetsManager->PlaySound2D(ESound::Slide);
	Camera->SetRelativeLocation(FVector(0.f, 0.f, SlideCameraZ));
	bIsSliding = true;
	GetCapsuleComponent()->SetCapsuleHalfHeight(SlideHeight);
	GetCharacterMovement()->GroundFriction = SlideGroundFriction;
	GetCharacterMovement()->BrakingFrictionFactor = SlideBrakingFrictionFactor;
	FVector SlideDir = GetLastMovementInputVector();
	if (SlideDir.IsNearlyZero()) SlideDir = GetActorForwardVector();
	LaunchCharacter(SlideDir.GetSafeNormal() * SlideStrength, true, false);
	GetWorld()->GetTimerManager().SetTimer(P_SlideEndHandle, this, &AEnemy::P_EndSlide, SlideDuration);
	Weapon->GetHandsAnimInstance()->PlayMontageWithHold(bIsSlidePrimaryAnim ? EHandsAnimState::Slide : EHandsAnimState::SlideSecondary, SlideDuration);
}

void AEnemy::P_EndSlide()
{
	Camera->SetRelativeLocation(FVector(0.f, 0.f, DefaultCameraZ));
	Weapon->GetHandsAnimInstance()->StopMontage(EHandsAnimState::Slide, 0.1f);
	Weapon->GetHandsAnimInstance()->StopMontage(EHandsAnimState::SlideSecondary, 0.1f);
	bIsSliding = false;
	AddActorWorldOffset(FVector(0.f, 0.f, GetDefaultHalfHeight() - SlideHeight));
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetDefaultHalfHeight());
	GetCharacterMovement()->GroundFriction = GroundFriction;
	GetCharacterMovement()->BrakingFrictionFactor = BrakingFrictionFactor;
}

void AEnemy::P_SetupSuperJump()
{
	GetWorldTimerManager().ClearTimer(P_SlideEndHandle);
	P_EndSlide();
	GetCharacterMovement()->JumpZVelocity = JumpVelocity * 2.f;
}

void AEnemy::P_ChangeFovWhileHijackingProcess()
{
	if (!HijackTarget || !HijackTarget->GetIsAlive())
	{
		P_CancelHijackingProcess();
		return;
	}
	Camera->SetFieldOfView(Camera->FieldOfView - 0.4f);
	if (Camera->FieldOfView <= 80.f)
	{
		Camera->FieldOfView = 80.f;
		GetWorldTimerManager().ClearTimer(HijackFovChangeHandle);
	}
}

void AEnemy::P_AddAndSetHijackPulse(const float InValue)
{
	if (!MID_HijackPulse) MID_HijackPulse = UMaterialInstanceDynamic::Create(MI_HijackPulse, this);
	GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::HijackPulse, MID_HijackPulse, InValue);
}

void AEnemy::P_LockInToTargetLogic() const
{
	if (!HijackTarget || !Camera) return;
	Camera->SetWorldRotation((HijackTarget->GetActorLocation() - Camera->GetComponentLocation()).Rotation());
}

void AEnemy::P_TurnOffInvincibility() { bIsInvincible = false; }

void AEnemy::P_OpenUpgradeSelection()
{
	if (!SaveSlotManager->GetRunSaveSlot()->IsUpgradeAvailable()) return;
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->OpenUI();
}

void AEnemy::P_SetupRunMode()
{
	if (!EnhancedInputComponent) return;
	const TObjectPtr<USettingsSave> SettingsSave = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	if (SettingsSave->GetIsRunToggleEnabled())
	{
		if (RunBindTurnOnHandle != 0)
		{
			EnhancedInputComponent->RemoveBindingByHandle(RunBindTurnOnHandle);
			EnhancedInputComponent->RemoveBindingByHandle(RunBindTurnOffHandle);
		}
		bPlayerRunModeIsToggle = true;
		RunBindTurnOnHandle = EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Started, this, &AEnemy::P_ToggleRun).GetHandle();
	}
	else
	{
		if (RunBindTurnOnHandle != 0) EnhancedInputComponent->RemoveBindingByHandle(RunBindTurnOnHandle);
		bPlayerRunModeIsToggle = false;
		RunBindTurnOnHandle = EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Started, this, &AEnemy::P_ToggleRun, true).GetHandle();
		RunBindTurnOffHandle = EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Completed, this, &AEnemy::P_ToggleRun, false).GetHandle();
	}
	P_ToggleRun(false);
}

void AEnemy::P_CheckStoppedMoving() const
{
	if (!RunVfx) return;
	if (GetVelocity().SizeSquared() < KINDA_SMALL_NUMBER) RunVfx->DeactivateImmediate();
	else if (bIsRunning) RunVfx->Activate();
}

void AEnemy::P_CancelHijackingProcess()
{
	Camera->FieldOfView = 90.f;
	SetInputState(false);
	P_SetLookLocked(false);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UHijackUI>(EWidget::HijackUI)->CloseUI();
	GetWorldTimerManager().ClearTimer(HijackDelayHandle);
	GetWorldTimerManager().ClearTimer(HijackFovChangeHandle);
	GetWorldTimerManager().ClearTimer(HijackLockInToTargetHandle);
	HijackTarget = nullptr;
	if (P_HUD) P_HUD->CancelHijackInitiateAnim();
	P_AddAndSetHijackPulse(0.f);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD)->CancelHijackInitiateAnim();
}

void AEnemy::P_AddAndSetBurnVfx()
{
	P_SetWarningIndicator(EWarningIndicator::Burn);
	GetWorldTimerManager().SetTimer(P_TriggerBurnIndicatorHandle, this, &AEnemy::P_TriggerBurnIndicator, 0.25f, true);
	AssetsManager->PlaySound2D(ESound::PlayerCaughtOnFire);
	AssetsManager->PlaySound2D(ESound::PlayerBurning, true);
	if (!MID_Burn) MID_Burn = UMaterialInstanceDynamic::Create(MI_Burn, this);
	GetWorldTimerManager().SetTimer(P_ProcessBurnVfxHandle, this, &AEnemy::P_ProcessBurnVfxForward, 0.025f, true);
	GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::PlayerBurnEffect, MID_Burn, 1.f);
}

void AEnemy::P_RemoveBurnVfx()
{
	AssetsManager->RemoveStoredSound(ESound::PlayerBurning);
	if (P_HUD) P_HUD->DisableWarningIndicator();
	GetWorldTimerManager().ClearTimer(P_TriggerBurnIndicatorHandle);
	if (!MID_Burn) return;
	GetWorldTimerManager().SetTimer(P_ProcessBurnVfxHandle, this, &AEnemy::P_ProcessBurnVfxReverse, 0.025f, true);
}

void AEnemy::P_ProcessBurnVfxForward()
{
	if (!MID_Burn) return;
	P_BurnVfxRadiusValue -= 0.1875f;
	if (P_BurnVfxRadiusValue <= 2.51f)
	{
		P_BurnVfxRadiusValue = 2.5f;
		GetWorldTimerManager().ClearTimer(P_ProcessBurnVfxHandle);
	}
	MID_Burn->SetScalarParameterValue(FName("Radius"), P_BurnVfxRadiusValue);
}

void AEnemy::P_ProcessBurnVfxReverse()
{
	if (!MID_Burn) return;
	P_BurnVfxRadiusValue += 0.1875f;
	if (P_BurnVfxRadiusValue >= 3.99f)
	{
		P_BurnVfxRadiusValue = 4.f;
		GetWorldTimerManager().ClearTimer(P_ProcessBurnVfxHandle);
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::PlayerBurnEffect, MID_Burn, 0.f);
	}
	MID_Burn->SetScalarParameterValue(FName("Radius"), P_BurnVfxRadiusValue);
}

void AEnemy::P_TriggerBurnIndicator() const
{
	if (!P_HUD) return;
	P_HUD->ToggleWarningIndicator();
}

void AEnemy::P_SetWarningIndicator(const EWarningIndicator InWarningIndicator)
{
	if (!P_HUD || CurrentWarningIndicator == EWarningIndicator::CriticalDamage) return;
	CurrentWarningIndicator = InWarningIndicator;
	P_HUD->SetWarningIndicator(InWarningIndicator);
}

void AEnemy::P_RemoveWeakSpots()
{
	for (int32 i = WeakSpots.Num() - 1; i >= 0; i--)
	{
		if (!WeakSpots[i]) continue;
		WeakSpots[i]->DestroyComponent();
		WeakSpots[i] = nullptr;
	}
	WeakSpots.Empty(); 
}

bool AEnemy::IsHackTerminalsGameMode() const { return DelegateManager->RequestCurrentGameMode.IsBound() && DelegateManager->RequestCurrentGameMode.Execute() == EGameMode::HackTerminalsMode; }

EDamageReason AEnemy::ReceiveDamageReason(const TObjectPtr<AActor>& InDamageCauser)
{
	if (!InDamageCauser) return EDamageReason::None;
	return InDamageCauser->Implements<UDamageReasonProvider>() ? Cast<IDamageReasonProvider>(InDamageCauser)->GetDamageReason() : EDamageReason::None;
}

void AEnemy::SetAnimInstance() { AnimInstance = Cast<UEnemyAnimationInstance>(GetMesh()->GetAnimInstance()); }

void AEnemy::TickAbilities(const float InDeltaTime)
{
	for (const TPair<int32, TObjectPtr<UAbility>> Ability : Abilities) Ability.Value->Tick(InDeltaTime);
	P_UpdateAbilitiesUI();
}

void AEnemy::SetupStimuliSource()
{
	StimuliSource = NewObject<UAIPerceptionStimuliSourceComponent>(this);
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterWithPerceptionSystem();
}

void AEnemy::ChangeXrayState(const bool bState)
{
	GetMesh()->SetRenderCustomDepth(bState);
	GetMesh()->SetCustomDepthStencilValue(bState);
	if (!DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::SpyAbility3, EUpgradeAttribute::Special1)) return;
	AI_ChangeWeakSpotsState(bState);
}

void AEnemy::SetupUserSettings() const
{
	const TObjectPtr<UEnhancedInputUserSettings> UserSettings = GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->GetUserSettings();
	if (!UserSettings) return;
	if (!UserSettings->IsMappingContextRegistered(PlayerMappingContext)) UserSettings->RegisterInputMappingContext(PlayerMappingContext);
}

void AEnemy::ProcessDamageReason(const EDamageReason InDamageReason, const float InDamage)
{
	switch (InDamageReason)
	{
		case EDamageReason::TricksterPoisonDamage: SaveSlotManager->GetSaveSlot()->ProgressVariationUnlock(EVariation::TricksterVariationPurple, InDamage); break;
		case EDamageReason::WraithPlayerMeteor:
			if (DelegateManager->RequestSpecialUpgradeAvailability.IsBound() && DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::WraithAbility3, EUpgradeAttribute::Special1))
			{
				SetupBurningState(EBurningType::Meteor, 2.f, 3.f, 0.2f);
			}
		break;
		case EDamageReason::BreacherDynamiteDamage:
			SetupBurningState(EBurningType::BreacherDynamite, 2.f, 3.f, 0.35f);
			break;
		default: ;
	}
}

void AEnemy::ProcessKillDamageReason(const EDamageReason InDamageReason)
{
	switch (InDamageReason)
	{
		case EDamageReason::GuardianDamage:
			if (DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::GuardianAbility2, EUpgradeAttribute::Special1))
			{
				if (DelegateManager->RequestIsTimeStopUsed.IsBound() && DelegateManager->RequestIsTimeStopUsed.Execute()) GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddGuardianTimeStopSpecialStack();
			}
			SaveSlotManager->GetSaveSlot()->ProgressVariationUnlock(EVariation::GuardianVariationGold, 1); break;
		case EDamageReason::RangerGrenadeDamage: SaveSlotManager->GetSaveSlot()->ProgressVariationUnlock(EVariation::RangerVariationBlue, 1); break;
		case EDamageReason::BreacherBashDamage: (void)DelegateManager->OnPlayerBashKill.ExecuteIfBound(); break;
		case EDamageReason::TankSlam: ImpulseAfterDeath = FVector::UpVector * 500.f; break;
		default: ;
	}
}

void AEnemy::CheckIfFallOutOfWorld() { if (GetActorLocation().Z <= -1000.f) bIsHijacked ? P_Death() : InitiateDeath(); }

void AEnemy::SpawnCondenseMaterialsProcess()
{
	if (!MI_MeshBaseMaterial) return;
	MeshBaseMaterialCondenseValue -= 0.015f;
	MeshEmissiveMaterialOpacityValue += 0.025f;
	MI_MeshBaseMaterial->SetScalarParameterValue("Progress", MeshBaseMaterialCondenseValue);
	if (MID_MeshEmissiveMaterial) MID_MeshEmissiveMaterial->SetScalarParameterValue("Opacity", MeshEmissiveMaterialOpacityValue);
	if (MeshEmissiveMaterialOpacityValue >= 0.5f) GetWorldTimerManager().ClearTimer(SpawnCondenseMaterialsProcessHandle);
}

void AEnemy::DestroyCondenseMaterialsProcess()
{
	if (!MI_MeshBaseMaterial) return;
	MeshBaseMaterialCondenseValue += 0.015f;
	MeshEmissiveMaterialOpacityValue -= 0.025f;
	MI_MeshBaseMaterial->SetScalarParameterValue("Progress", MeshBaseMaterialCondenseValue);
	if (MID_MeshEmissiveMaterial) MID_MeshEmissiveMaterial->SetScalarParameterValue("Opacity", MeshEmissiveMaterialOpacityValue);
	if (MeshEmissiveMaterialOpacityValue <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(DestroyCondenseMaterialsProcessHandle);
		DestroyImmediately();
	}
}

void AEnemy::OpenInfoPanel() { /*if (P_HUD) P_HUD->ToggleInfoPanel(true);*/ }

void AEnemy::CloseInfoPanel() { /*if (P_HUD) P_HUD->ToggleInfoPanel(false);*/ }

void AEnemy::PlayEmpAffectedVfx(const float InEffectDuration)
{
	if (!EmpAffectedVfx) AssetsManager->AttachVfx(EVfx::EmpAffected, [this]() -> bool { return GetIsAlive(); }, &EmpAffectedVfx, GetMesh(), FName("DeathSocket"), false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, EVariation::Default, {{"Loop Duration", FNiagaraAttribute(EUserParamType::Float, InEffectDuration)}});
	else EmpAffectedVfx->Activate(true);
}

void AEnemy::DisableHijackViableVfx() const { if (HijackableVfx) HijackableVfx->DeactivateImmediate(); }

void AEnemy::DisableSpySnatchMarkerVfx() const { if (SpySnatchMarkerVfx) SpySnatchMarkerVfx->DeactivateImmediate(); }

void AEnemy::DisableVfxs() const
{
	DisableHijackViableVfx();
	DisableSpySnatchMarkerVfx();
	if (LevelUpVfx) LevelUpVfx->DeactivateImmediate();
	if (RunVfx) RunVfx->DeactivateImmediate();
}

void AEnemy::SetupTutorialManager()
{
	const TObjectPtr<UTutorialManager> TutorialManager = GetWorld()->GetSubsystem<UTutorialManager>();
	if (!TutorialManager->GetIsInitialized()) TutorialManager->Init();
	TutorialManager->SetPlayerRef(this);
	bIsImmortal = true;
}

void AEnemy::ToggleTimeDilation()
{
	if (!FMath::IsNearlyEqual(UGameplayStatics::GetGlobalTimeDilation(GetWorld()), GameInstance->GetGlobalTimeDilation(), 0.01f)) UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GameInstance->GetGlobalTimeDilation());
	else UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

void AEnemy::SetupMeshOptimization() const
{
	GetCharacterMovement()->SetComponentTickInterval(1.f / MovementFramesPerSecond);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	GetMesh()->bEnableUpdateRateOptimizations = true;
	GetMesh()->SkipUpdateOverlapsOptimEnabled = true;
	GetMesh()->SetAllowAnimCurveEvaluation(false);
	GetMesh()->bUseRefPoseOnInitAnim = true;
	GetMesh()->SetComponentTickInterval(1.f / 30.f);
	GetCharacterMovement()->bComponentShouldUpdatePhysicsVolume = false;
	GetCharacterMovement()->bAutoRegisterPhysicsVolumeUpdates = false;
}

void AEnemy::AddAdditionalActionSpeed(const float InValue, const float InDuration)
{
	if (InDuration > 0.f) GetWorldTimerManager().SetTimer(ActionSpeedHandle, this, &AEnemy::ResetAdditionalActionSpeed, InDuration);
	AdditionalActionSpeedValue += InValue;
	SetActionSpeed(ActionSpeed);
}

void AEnemy::ResetAdditionalActionSpeed()
{
	ActionSpeed -= AdditionalActionSpeedValue;
	AdditionalActionSpeedValue = 0.f;
	SetActionSpeed(ActionSpeed);
}

void AEnemy::ResetActionSpeed()
{
	SlowingStacks = 0;
	ActionSpeed = 1.f;
	AdditionalActionSpeedValue = 0.f;
	SetActionSpeed(ActionSpeed);
}

void AEnemy::ApplyStunEffect(const float InEffectDuration)
{
	if (FMath::IsNearlyEqual(InEffectDuration, 0.f, 0.01f)) return;
	if (!bIsHijacked)
	{
		bAffectedByStun = true;
		PlayStunAffectedVfx(InEffectDuration);
		GetWorldTimerManager().SetTimer(StunEffectEndHandle, this, &AEnemy::AI_StunEffectEnded, InEffectDuration);
		if (!AnimInstance) return;
		if (DisablingActionsEffectsCount < 2) AnimInstance->PlayMontageWithHold(EEnemyAnimState::StunEffect, InEffectDuration, true);
		AnimInstance->SetNonBlendableSlotState(true);
		AnimInstance->SetIsControlRigEnabled(false);
	}
	else
	{
		DelegateManager->RequirePlayerAnnouncementInfo.ExecuteIfBound(NSLOCTEXT("ST_Localization", "Stunned", "Stunned"));
		P_MakeCameraShake(ECameraShake::PlayerStunned);
		SetInputState(true);
		P_SetLookLocked(true);
		if (Weapon && Weapon->GetHandsAnimInstance()) Weapon->GetHandsAnimInstance()->PlayMontageWithHold(EHandsAnimState::StunEffect, InEffectDuration);
		GetWorldTimerManager().SetTimer(StunEffectEndHandle, this, &AEnemy::P_StunEffectEnded, InEffectDuration);
	}
}

void AEnemy::PlayStunAffectedVfx(const float InEffectDuration)
{
	if (!StunAffectedVfx)
	{
		AssetsManager->AttachVfx(EVfx::Stunned, [this]() -> bool { return GetIsAlive(); }, &StunAffectedVfx, GetMesh(),
		FName("DeathSocket"), false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget,
		false, EVariation::Default, {{"Loop Duration", FNiagaraAttribute(EUserParamType::Float, InEffectDuration)}});
	}
	else
	{
		TMap<FName, FNiagaraAttribute> Attributes;
		Attributes.Add(FName("Loop Duration"), FNiagaraAttribute(EUserParamType::Float, InEffectDuration));
		AssetsManager->UpdateVfxAttributes(StunAffectedVfx, Attributes);
		StunAffectedVfx->Activate(true);
	}
}

void AEnemy::ApplyDisablingActionEffect(const float InEffectDuration, const EDisablingEffect InEffectName)
{
	GetWorldTimerManager().ClearTimer(RecoverFromDisablingEffectHandle);
	switch (InEffectName)
	{
		case EDisablingEffect::Emp: if (!bAffectedByEmp) DisablingActionsEffectsCount++; break;
		case EDisablingEffect::Stun: if (!bAffectedByStun) DisablingActionsEffectsCount++;
	}
	AI_DisableActions(InEffectDuration, InEffectName);
}

void AEnemy::DisableSkeletalMesh() const
{
	GetMesh()->PrimaryComponentTick.bCanEverTick = false;
	GetMesh()->SetComponentTickEnabled(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(false);
	GetMesh()->SetNotifyRigidBodyCollision(false);
}

void AEnemy::StopBurningState()
{
	if (bIsHijacked) P_RemoveBurnVfx();
	BurningLogicData.StopBurnState();
	GetWorldTimerManager().ClearTimer(BurningTickingHandle);
}

void AEnemy::TogglePoisonSlowVfx(const bool bState)
{
	if (!PoisonSlowAffectedVfx)
	{
		if (bPoisonSlowVfxAttachRequested) return;
		bPoisonSlowVfxAttachRequested = true;
		bIsPoisonSlowAffected = true;
		AssetsManager->AttachVfx(EVfx::PoisonSlowAffected, [this]() -> bool { return GetIsAlive() && bIsPoisonSlowAffected; }, &PoisonSlowAffectedVfx, GetMesh(), FName("DeathSocket"));
	}
	else
	{
		if (bState) PoisonSlowAffectedVfx->Activate(true);
		else
		{
			bIsPoisonSlowAffected = false;
			PoisonSlowAffectedVfx->DeactivateImmediate();
		}
	}
}
