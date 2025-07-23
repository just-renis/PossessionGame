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
#include "PossessionGame/AI/Navigation/NavAreaModifier.h"
#include "PossessionGame/Data Assets/AI/Base Data/AICharacterData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerCharacterData.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Destructibles/Destructible.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EUIBar.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/UI/MainWidgets/PauseMenuUI.h"
#include "PossessionGame/UI/MainWidgets/PlayerGhostUI.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/UI/MainWidgets/SettingsUI.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/UI/MainWidgets/VariationsUI.h"
#include "PossessionGame/UI/SecondaryWidgets/AIHpBarUI.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameGameModeBase.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/AbilityManager.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
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
}

bool AEnemy::GetIsHijackAble() const { return AI_Data->GetIsPossessable(); }

float AEnemy::GetSkeletalMeshHeight() const { return GetMesh()->GetSocketLocation(TEXT("Head")).Z - GetMesh()->GetComponentLocation().Z; }

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

void AEnemy::AI_UseAbility(const int32 AbilityIndex) { if (AbilityIndex < Abilities.Num()) Abilities[AbilityIndex]->AI_Execute(); }

void AEnemy::AI_SetMoveMode(const EMoveMode InMoveMode)
{
	CurrentMoveMode = InMoveMode;
	SetMaxWalkSpeed(InMoveMode == EMoveMode::Walk ? AI_Data->GetMaxWalkSpeed() : AI_Data->GetMaxRunSpeed());
}

void AEnemy::AI_SetShootPause()
{
	bShootPauseActive = true;
	GetWorldTimerManager().SetTimer(AI_ClearShootPauseHandle, this, &AEnemy::AI_ClearShootPause, AI_Data->GetShootPause(), false);
}

void AEnemy::P_ApplyUpgradeOnSelection(const EUpgrade InUpgradeName) // For future upgrades to increase dash charges, stomping damage and so on.
{
	bInUpgradeSelection = false;
	for (int32 i = 0; i < Abilities.Num(); i++) Abilities[i]->SetupUpgradeAppliance();
}

void AEnemy::P_UseAbility(const int32 AbilityIndex)
{
	if (bIsInputLocked) return;
	if (!bIsActionInMotion && AbilityIndex < Abilities.Num() && Abilities[AbilityIndex]->CanExecute())
	{
		Abilities[AbilityIndex]->P_Execute();
		if (!P_HUD) return;
		if (Abilities[AbilityIndex]->GetIsCooldownKill()) P_HUD->UpdateAbilityUI(Abilities[AbilityIndex]);
	}
}

void AEnemy::P_UpdateAmmoHUD(const int32 WeaponAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(WeaponAmmo); }

void AEnemy::P_UpdateAmmoHUD(const int32 WeaponAmmo, const int32 MaxAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(WeaponAmmo, MaxAmmo); }

void AEnemy::P_UpdateAmmoHUD(const int32 FirstWeaponAmmo, const int32 SecondWeaponAmmo, const int32 MaxAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(FirstWeaponAmmo, SecondWeaponAmmo, MaxAmmo); }

void AEnemy::P_StopTime()
{
	if (FMath::IsNearlyEqual(UGameplayStatics::GetGlobalTimeDilation(GetWorld()), 1.f, 0.01f)) UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GameInstance->GetTimeDilation());
	else UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

void AEnemy::P_SetFov(const float InFov)
{
	TargetFov = InFov;
	FovChangeTick = (TargetFov - Camera->FieldOfView) * 0.1f;
	GetWorldTimerManager().SetTimer(P_ContinuousFovChangeHandle, this, &AEnemy::P_ContinuousFovChange, 0.025f, true);
}

void AEnemy::P_IncreaseMovementSpeed(const float InPercentage) const { SetMaxWalkSpeed(P_Data->GetMaxWalkSpeed() + P_Data->GetMaxWalkSpeed() * InPercentage * 0.01f); }

void AEnemy::P_ResetMovementSpeed() const { SetMaxWalkSpeed(P_Data->GetMaxWalkSpeed()); }

void AEnemy::SetIsActionInMotion(const bool bState)
{
	bIsActionInMotion = bState;
	if (AI) AI->GetStateTreeData().SetIsActionInMotion(bState);
}

void AEnemy::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsAlive)
	{
		if (!bInUpgradeSelection && Abilities.Num() > 0) TickAbilities(DeltaTime);
		if (P_HUD) P_HUD->UpdateFps(FMath::RoundToInt32(1.f / DeltaTime));
	}
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
	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(0).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Move);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(4).Action, ETriggerEvent::Started, this, &AEnemy::P_Jump);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(5).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Rotate);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Dash);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(7).Action, ETriggerEvent::Triggered, this, &AEnemy::P_HijackEnemy);
		P_SetupLeftClickInput(EnhancedInputComponent);
		if (bCanReload) EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(9).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Reload);
		#if WITH_EDITOR
			EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(10).Action, ETriggerEvent::Started, this, &AEnemy::P_ToggleGodMode);
		#endif
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(11).Action, ETriggerEvent::Started, this, &AEnemy::P_UseAbility, 0);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(12).Action, ETriggerEvent::Started, this, &AEnemy::P_UseAbility, 1);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(13).Action, ETriggerEvent::Started, this, &AEnemy::P_UseAbility, 2);
		P_SetupRightClickInput(EnhancedInputComponent);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &AEnemy::P_HandleEscapeClick);
		#if WITH_EDITOR
			EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(16).Action, ETriggerEvent::Started, this, &AEnemy::P_StopTime);
		#endif
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(17).Action, ETriggerEvent::Started, this, &AEnemy::P_TryInteracting);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(18).Action, ETriggerEvent::Started, this, &AEnemy::P_QuickMeleeAttack);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(19).Action, ETriggerEvent::Started, this, &AEnemy::P_Stomp);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(20).Action, ETriggerEvent::Started, this, &AEnemy::OpenInfoPanel);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(20).Action, ETriggerEvent::Completed, this, &AEnemy::CloseInfoPanel);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(21).Action, ETriggerEvent::Started, this, &AEnemy::P_Crouch);
	}
}

void AEnemy::PossessedBy(AController* NewController)
{
	if (!bIsHijacked)
	{
		Super::PossessedBy(NewController);
		if (Cast<APlayerController>(NewController))
		{
			DisableHijackViableVfx();
			(void)DelegateManager->SendTargetToTutorialDrone.ExecuteIfBound(this);
			bIsActionInMotion = false;
			P_TriggerGlitchingEffect();
			P_ToggleCriticallyDamagedEffects(0.f);
			AssetsManager->PlaySound2D(ESound::OnHijack);
			AssetsManager->RemoveStoredSound(ESound::GuardianSlowMotion);
			AssetsManager->RemoveStoredSound(ESound::CriticalDamageBeep);
			GetWorldTimerManager().SetTimer(AI_TriggerDeathVoiceLineHandle, this, &AEnemy::AI_TriggerDeathVoiceLine, 1.f, false);
			P_Setup();
			DelegateManager->RequirePlayerAnnouncementInfo.ExecuteIfBound(FText::FromString("Hijacked"));
			SetInputState(false);
		}
	}
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

void AEnemy::AI_StopAbility(const int32 InIndex) { if (Abilities.IsValidIndex(InIndex)) Abilities[InIndex]->AI_StopAbility(); }

void AEnemy::AI_TryEvolving()
{
	if (FMath::FRand() <= AI_Data->GetMutationChance())
	{
		bIsMutant = true;
		MaxHp -= MaxHp * 0.01f * AI_Data->GetMutantHpIncreasePercentage();
		AssetsManager->AttachVfx(EVfx::MutantAura, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &Aura, GetMesh(), FName("MutantAuraSocket"));
	}
	AI_SetupUpgradesAppliance();
}

void AEnemy::AI_SetupUpgradesAppliance()
{
	MaxHp += GetWorld()->GetSubsystem<UUpgradeManager>()->AI_GetTotalUpgradeAttributeValue(EUpgradeAttribute::FlatHealth);
	CurrentHp = MaxHp;
}

void AEnemy::InitiateDeath()
{
	DisableHijackViableVfx();
	ClearTimers();
	AI_DestroyHpBar();
	bIsAlive = false;
	Tags.Remove("AliveEnemy");
	if (!bIsHijacked)
	{
		AI_SpawnUsbOnDeath();
		AI_PlayDeathSound();
		AI_PlayDeathExplosion();
		DestroyAI();
		if (bIsMutant) AI_TriggerMutantExplosion();
		if (AI_Data) (void)DelegateManager->OnEnemyDeath.ExecuteIfBound(AI_Data->GetXp(), AI_Data->GetPossessionTimeValue());
		(void)DelegateManager->UpdateGameModeEnemiesAmount.ExecuteIfBound();
	}
	else P_Destroy();
	AI_PlayDeathElectrification();
	HandleDeactivation();
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AEnemy::DestroyThis, DestroyAfter, false);
}

void AEnemy::ReceiveDamage(const float InDamage, const EDamageReason InDamageReason)
{
	const float TrueDamage = InDamage * DamageReduction;
	CurrentHp -= TrueDamage;
	if (P_HUD) P_HandlingDamageTaken();
	else if (bIsAlive)
	{
		AI_ReceivedDamage(InDamageReason, TrueDamage);
		AI_ShowDamageNumber(TrueDamage);
	}
	if (bIsAlive && CurrentHp <= 0)
	{
		if (bIsHijacked) P_Death();
		else ProcessKillDamageReason(InDamageReason);
		InitiateDeath();
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

void AEnemy::SetEmpEffect(const float InEffectDuration)
{
	if (!AnimInstance) return;
	if (bAffectedByEmp)
	{
		AnimInstance->ClearMontageWithHoldHandle();
		AnimInstance->StopAllMontages(0.f);
	}
	bAffectedByEmp = true;
	PlayEmpAffectedVfx(InEffectDuration);
	PlaySound(ESound::EmpEffectApplied, true);
	AnimInstance->PlayMontageWithHold(EEnemyAnimState::EmpEffect, InEffectDuration, true);
}

void AEnemy::SetHijackOutline(const bool bState)
{
	bHijackOutlineState = bState;
	if (bState)
	{
		if (!HijackViable)
		{
			AssetsManager->AttachVfx(EVfx::HijackViable, [this]() -> bool { return GetIsAlive() && bHijackOutlineState; }, &HijackViable, GetMesh(), FName("RootSocket"));
		}
		else
		{
			if (HijackViable->IsActive()) HijackViable->Deactivate();
			HijackViable->Activate();
		}
	}
	else DisableHijackViableVfx();
	GetMesh()->SetOverlayMaterial(bState ? HijackOutlineMaterial : nullptr);
}

void AEnemy::SetUnHijackOutline(const bool bState) const
{
	GetMesh()->SetOverlayMaterial(bState ? UnHijackOutlineMaterial : nullptr);
}

void AEnemy::EmpEffectEnded()
{
	bAffectedByEmp = false;
	SetIsActionInMotion(false);
	if (AI) AI->ResumeLogic();
}

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
			Abilities.Add(Ability);
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
}

void AEnemy::AI_SetupWeapon()
{
	Weapon->SetProprietor(this);
	AI->SetShootCooldown(Weapon->GetShootCooldown());
	Weapon->PostProprietorSetup();
}

void AEnemy::AI_Shoot() { Weapon->AI_Shoot(); }

void AEnemy::AI_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName)
{
	if (!AI_Data) AI_Data = AssetsManager->GetCharacterData(PrimaryDataAssetName).GetCharacterContainer()->AI_Data.LoadSynchronous();
}

void AEnemy::AI_SetController() { AI = Cast<AEnemyAIController>(GetController()); }

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
	if (Weapon->GetAnimInstance() && Weapon->GetAnimInstance()->IsMontageExisting(EWeaponAnimState::AIReload)) Weapon->GetAnimInstance()->PlayMontage(EWeaponAnimState::AIReload);
	Weapon->AI_Reload();
	Weapon->AI_ResetShootCycles();
}

void AEnemy::AI_PlayDeathSound() { PlaySound(ESound::MechanicalDeath); }

void AEnemy::P_SetupAbilities()
{
	Abilities.Empty();
	TArray<int32> ChosenAbilities = GameInstance->GetSaveSlot()->GetCharacterChosenAbilities(CharacterName).Abilities;
	const TMap<EAbility, TSoftObjectPtr<UAbilityData>> AbilityData = AssetsManager->GetCharacterData(CharacterName).GetCharacterContainer()->P_Abilities_Data;
	const TObjectPtr<UAbilityManager> AbilityManager = GameInstance->GetSubsystem<UAbilityManager>();
	for (int32 i = 0; i < ChosenAbilities.Num(); i++)
	{
		if (TObjectPtr<UAbility> Ability = AbilityManager->GetAbility(this, CharacterName, ChosenAbilities[i]))
		{
			if (P_HUD) P_HUD->SetAbilityIcon(i, ChosenAbilities[i]);
			Ability->Setup(this, AbilityData[AbilityManager->GetAbilityName(CharacterName, ChosenAbilities[i])].LoadSynchronous(), DelegateManager);
			Ability->SetAbilityIndex(i);
			Abilities.Add(Ability);
		}
	}
}

void AEnemy::P_Setup()
{
	DelegateManager->ApplyUpgradeOnSelection.BindUObject(this, &AEnemy::P_ApplyUpgradeOnSelection);
	DelegateManager->OnUpgradeSelectionOpened.BindUObject(this, &AEnemy::P_InUpgradeSelection);
	DelegateManager->OnShowPressKeyToContinue.AddUObject(this, &AEnemy::SetPressKeyState);
	DelegateManager->OnMinionBossStomp.BindUObject(this, &AEnemy::P_MakeCameraShake);
	DelegateManager->OnVariationSelectionEnd.BindUObject(this, &AEnemy::P_OnVariationSelection);
	DelegateManager->OnLevelUp.AddUObject(this, &AEnemy::P_SetupLevelUpAppliance);
	DelegateManager->OnXpReceived.BindUObject(this, &AEnemy::P_AddXp);
	DelegateManager->ApplySecretBoostHealth.BindUObject(this, &AEnemy::P_ApplySecretBoostHp);
	Tags.Remove("AliveEnemy");
	Tags.Add("Player");
	P_ReceiveCurrentlySelectedVariation();
	DestroyAI();
	GetMesh()->SetCollisionProfileName("Player");
	GetMesh()->SetGenerateOverlapEvents(true);
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->SetTarget(this);
	P_LoadDataIfNotLoaded(CharacterName);
	bUseControllerRotationYaw = true;
	SetupStimuliSource();
	P_SetupNavArea();
	P_SetupData();
	P_SetupCharacterMovement();
	Camera->SetActive(true);
	GetWorldTimerManager().SetTimer(P_ControlLogicHandle, this, &AEnemy::P_ControlLogic, 0.05f, true);
	Weapon->P_SetupTransfer();
	GetMesh()->SetHiddenInGame(true);
	P_InputManager = Cast<UEnhancedPlayerInput>(GetWorld()->GetFirstPlayerController()->PlayerInput);
	P_SetupUI();
	P_SetupAbilities();
	if (AI_Data) (void)DelegateManager->OnEnemyDeath.ExecuteIfBound(AI_Data->GetXp(), AI_Data->GetPossessionTimeValue());
	(void)DelegateManager->UpdateGameModeEnemiesAmount.ExecuteIfBound();
	DelegateManager->OnEnemyDeath.BindUObject(this, &AEnemy::P_OnEnemyDeathEvents);
}

void AEnemy::P_Shoot(const bool bFirstWeapon)
{
	if (bIsInputLocked || bIsActionInMotion || !bCanShoot) return;
	if (Weapon->P_CanShoot(bFirstWeapon)) Weapon->P_Shoot(bFirstWeapon);
	else if (Weapon->P_CanReload() && Weapon->GetCurrentAmmoAmount() <= 0) Weapon->P_Reload();
}

void AEnemy::P_SetupLeftClickInput(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	P_SetupLeftClickInputStartedState(LeftClickInputComponent);
	P_SetupLeftClickInputTriggeredState(LeftClickInputComponent);
	P_SetupLeftClickInputCompletedState(LeftClickInputComponent);
}

void AEnemy::P_SetupLeftClickInputTriggeredState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Shoot, true);
}

void AEnemy::P_SetupRightClickInput(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	P_SetupRightClickInputStartedState(RightClickInputComponent);
	P_SetupRightClickInputTriggeredState(RightClickInputComponent);
	P_SetupRightClickInputCompletedState(RightClickInputComponent);
}

void AEnemy::P_Destroy()
{
	DelegateManager->OnMinionBossStomp.Unbind();
	GetMesh()->SetHiddenInGame(false);
	StimuliSource->UnregisterFromPerceptionSystem();
	Weapon->SetToOriginalSettings();
	GetMesh()->SetCollisionProfileName("Enemy");
	NavAreaModifier->Destroy();
	if (bPlayerDied)
	{
		GameInstance->GetSaveSlot()->SaveSlot();
		GameInstance->ResetRunSaveSlot();
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
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = bCanCrouch;
	GetCharacterMovement()->SetCrouchedHalfHeight(CrouchHalfHeight);
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxWalkSpeedCrouched;
	GetCharacterMovement()->GravityScale = GravityScale;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->AirControl = AirControl;
	GetCharacterMovement()->MaxAcceleration = MaxAcceleration;
	GetCharacterMovement()->BrakingFrictionFactor = BrakingFrictionFactor;
	GetCharacterMovement()->GetCharacterOwner()->JumpMaxCount = MaxJumpAmount;
	GetCharacterMovement()->bUseRVOAvoidance = false;
}

void AEnemy::P_Move(const FInputActionValue& Value)
{
	if (bIsInputLocked) return;
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AEnemy::P_InitiateDash()
{
	bCanDash = false;
	if (P_HUD) P_HUD->UpdateDashIndicator(false);
	AssetsManager->PlaySound2D(ESound::Dash);
	CurrentDashCooldown = 0.f;
	FVector DashDirection = FVector(GetVelocity().X, GetVelocity().Y, 0.f).GetSafeNormal();
	if (DashDirection.IsNearlyZero()) DashDirection = GetActorForwardVector();
	DashDirection.Z = 0.f;
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(DashDirection * P_Data->GetDashStrength(), true, true);
	GetWorldTimerManager().SetTimer(P_StopDashHandle, this, &AEnemy::P_StopDash, 0.2f, false);
	if (GameInstance->GetSaveSlot()->GetIsTutorialFinished()) return;
	if (GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerGhostUI>(EWidget::TutorialUI, 0)->GetCurrentHintIndex() == 3) (void)DelegateManager->OnShowHint.ExecuteIfBound(4);
}

void AEnemy::P_InitiateJump()
{
	Jump();
	Weapon->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::Jump);
	const bool bIsDoubleJump = GetCharacterMovement()->GetCharacterOwner()->JumpCurrentCount == 1;
	AssetsManager->PlaySound2D(bIsDoubleJump ? ESound::DoubleJump : ESound::Jump);
	if (!bIsDoubleJump) return;
	bCanStomp = true;
	if (GameInstance->GetSaveSlot()->GetIsTutorialFinished()) return;
	if (GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerGhostUI>(EWidget::TutorialUI, 0)->GetCurrentHintIndex() == 4) (void)DelegateManager->OnShowHint.ExecuteIfBound(5);
}

void AEnemy::P_InitiateStomp()
{
	bIsStomping = true;
	AssetsManager->PlaySound2D(ESound::PlayerStompDashDown);
	GetCharacterMovement()->AirControl = 0.f;
	FVector Velocity = GetCharacterMovement()->Velocity;
	Velocity.Z = 0.f;
	GetCharacterMovement()->Velocity = Velocity + FVector(0.f, 0.f, -3000.f);
	if (GameInstance->GetSaveSlot()->GetIsTutorialFinished()) return;
	(void)DelegateManager->StopShowingHint.ExecuteIfBound();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UPossessionGameInstance>(GetWorld()->GetGameInstance());
	DelegateManager = GameInstance->GetSubsystem<UDelegateManager>();
	GameModeBase = Cast<APossessionGameGameModeBase>(GetWorld()->GetAuthGameMode());
	GlobalSettingsManager = GameInstance->GetSubsystem<UGlobalSettingsManager>();
	AI_SetupHpBar();
	AssetsManager = GameInstance->GetSubsystem<UPrimaryDataAssetsManager>();
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->AddEnemy(this);
	SetMesh(CharacterName);
	ProcessSpawnEffects();
	SetAnimInstance();
	Tags.Add("Enemy");
	Tags.Add("AliveEnemy");
	Camera->SetActive(false);
	GetWorldTimerManager().SetTimer(CheckIfFallOutOfWorldHandle, this, &AEnemy::CheckIfFallOutOfWorld, 2.f, true);
	GetWorldTimerManager().SetTimer(AI_SetCharacterMovementSpeedHandle, this, &AEnemy::AI_SetCharacterMovementSpeed, 0.05f, true);
	GetWorldTimerManager().SetTimer(AI_PlaySpawnSoundHandle, this, &AEnemy::AI_PlaySpawnSound, 3.f + UKismetMathLibrary::RandomInteger(4), false);
	DelegateManager->OnXRayThroughWallsChange.AddUObject(this, &AEnemy::ChangeXRayState);
	AI_Setup();
	SetInputState(false);
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AnimInstance) AnimInstance->ClearTimers();
	GetWorldTimerManager().ClearTimer(AI_ClearShootPauseHandle);
	GetWorldTimerManager().ClearTimer(AI_TriggerDeathVoiceLineHandle);
	GetWorldTimerManager().ClearTimer(AI_SetCharacterMovementSpeedHandle);
	GetWorldTimerManager().ClearTimer(AI_PlaySpawnSoundHandle);
	GetWorldTimerManager().ClearTimer(AI_HideHpBarHandle);
	GetWorldTimerManager().ClearTimer(AI_DamageTakenEffectHandle);
	GetWorldTimerManager().ClearTimer(P_ContinuousFovChangeHandle);
	GetWorldTimerManager().ClearTimer(P_ControlLogicHandle);
	GetWorldTimerManager().ClearTimer(P_StopDashHandle);
	GetWorldTimerManager().ClearTimer(P_DetectIfPlayerPointingAtOutlineObjectsHandle);
	GetWorldTimerManager().ClearTimer(P_TriggerCriticalDamageIndicatorHandle);
	GetWorldTimerManager().ClearTimer(P_DeathHandle);
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	GetWorldTimerManager().ClearTimer(CheckIfFallOutOfWorldHandle);
	GetWorldTimerManager().ClearTimer(DestroyCondenseMaterialsProcessHandle);
	GetWorldTimerManager().ClearTimer(SpawnCondenseMaterialsProcessHandle);
	GetWorldTimerManager().ClearTimer(RestartLevelHandle);
	Super::EndPlay(EndPlayReason);
}

void AEnemy::SetAnimInstance() { AnimInstance = Cast<UEnemyAnimationInstance>(GetMesh()->GetAnimInstance()); }

void AEnemy::SetMaxWalkSpeed(const float Speed) const
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.GetSafeNormal() * Speed;
}

void AEnemy::SetInitialWalkSpeed(const bool bState) const { SetMaxWalkSpeed(bState ? P_Data->GetMaxWalkSpeed() : P_Data->GetMaxSlowedSpeed()); }

void AEnemy::AttachDestructible(const TObjectPtr<ADestructible>& Destructible, const FName SocketName) const
{
	Destructible->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AEnemy::HandleDeactivation()
{
	if (Weapon) Weapon->DeactivateWeapon();
	GetCapsuleComponent()->DestroyComponent();
	if (AnimInstance) AnimInstance->ClearTimers();
	GetMesh()->SetAnimInstanceClass(nullptr);
	GetMesh()->SetSimulatePhysics(true);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	const FRadialDamageEvent* RadialDamageEvent = nullptr;
	const FPointDamageEvent* PointDamageEvent = nullptr;
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
	else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
	ReceiveDamage(Damage, DamageCauser->Implements<UDamageReasonProvider>() ? Cast<IDamageReasonProvider>(DamageCauser)->GetDamageReason() : EDamageReason::None);
	if (GetMesh()->IsSimulatingPhysics())
	{
		if (RadialDamageEvent) GetMesh()->AddRadialImpulse(RadialDamageEvent->Origin, RadialDamageEvent->Params.OuterRadius, 200000.f, RIF_Linear);
		else if (PointDamageEvent) GetMesh()->AddImpulseAtLocation(PointDamageEvent->ShotDirection * 10000.f, PointDamageEvent->HitInfo.Location);
	}
	return Damage;
}

void AEnemy::SetMesh(const ECharacterType PrimaryDataAssetName) { GetMesh()->SetSkeletalMesh(AssetsManager->GetCharacterData(PrimaryDataAssetName).GetCharacterContainer()->CharacterMesh.Get()); }

void AEnemy::DestroyAI()
{
	if (Aura) Aura->Deactivate();
	if (EmpAffected) EmpAffected->Deactivate();
	if (AI)
	{
		GetWorld()->GetSubsystem<UEnemyCommunicator>()->RemoveEnemy(this);
		DelegateManager->OnXRayThroughWallsChange.RemoveAll(this);
		ChangeXRayState(false);
		AI_StopAllAbilities();
		GetMesh()->SetAnimInstanceClass(nullptr);
		AI->DestroyThis();
		AI->UnPossess();
		AI->Destroy();
	}
}

void AEnemy::DestroyThis()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnLevelUp.RemoveAll(this);
	if (Weapon) Weapon->DestroyThis();
	GetWorldTimerManager().SetTimer(DestroyCondenseMaterialsProcessHandle, this, &AEnemy::DestroyCondenseMaterialsProcess, 0.05f, true);
}

void AEnemy::AI_SetupData()
{
	CurrentHp = MaxHp = AI_Data->GetMaxHp();
	SetMaxWalkSpeed(AI_Data->GetMaxWalkSpeed());
}

void AEnemy::AI_StopAllAbilities() { for (int32 i = 0; i < Abilities.Num(); i++) Abilities[i]->AI_StopAbility(); }

void AEnemy::AI_SetupHpBar()
{
	if (!AI_HpBarComponent) return;
	AI_HpBar = Cast<UAIHpBarUI>(AI_HpBarComponent->GetWidget());
	AI_HideHpBar();
}

void AEnemy::AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage)
{
	if (MI_MeshBaseMaterial) MI_MeshBaseMaterial->SetScalarParameterValue("RedFlash", 1.f);
	GetWorldTimerManager().SetTimer(AI_DamageTakenEffectHandle, this, &AEnemy::AI_DisableDamageTakenRedFlash, 0.02f, false);
	ProcessDamageReason(InDamageReason, InDamage);
	if (!AI_HpBar) return;
	AI_HpBar->UpdateHpBar(CurrentHp, MaxHp);
	AI_HpBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetWorldTimerManager().SetTimer(AI_HideHpBarHandle, this, &AEnemy::AI_HideHpBar, 2.f, false);
}

void AEnemy::AI_DestroyHpBar()
{
	GetWorldTimerManager().ClearTimer(AI_HideHpBarHandle);
	if (!AI_HpBar || !AI_HpBarComponent) return;
	AI_HpBarComponent->DestroyComponent();
	AI_HpBar = nullptr;
	AI_HpBarComponent = nullptr;
}

void AEnemy::AI_ShowDamageNumber(const float InDamageAmount) const
{
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::DamageNumber, GetActorLocation() + FVector(0.f, 0.f, 60.f) + GetActorForwardVector() * 50.f, FRotator::ZeroRotator)->Init(InDamageAmount);
}

void AEnemy::AI_SpawnUsbOnDeath() const
{
	if (!GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetIsUsbDroppable()) return;
	if (FMath::FRand() <= AI_Data->GetUsbDropChance())
	{
		GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::Usb, GetActorLocation())->Init();
		GetWorld()->GetSubsystem<UEnemyCommunicator>()->SetIsUsbDroppable(false);
	}
}

void AEnemy::AI_TriggerMutantExplosion()
{
	AssetsManager->SpawnVfx(EVfx::MutantExplosion, nullptr, GetActorLocation());
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, GetActorLocation(), FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(200.f));
	TSet<TObjectPtr<AActor>> UniqueActors;
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> HitActor = Result.GetActor();
		if (!HitActor || UniqueActors.Contains(HitActor)) continue;
		UniqueActors.Add(HitActor);
		if (HitActor->ActorHasTag("Player")) UGameplayStatics::ApplyDamage(HitActor, MutantExplosionDamage, nullptr, this, UDamageType::StaticClass());
	}
}

void AEnemy::AI_HideHpBar() const { if (AI_HpBar) AI_HpBar->SetVisibility(ESlateVisibility::Collapsed); }

void AEnemy::AI_ClearShootPause()
{
	bShootPauseActive = false;
	GetWorldTimerManager().ClearTimer(AI_ClearShootPauseHandle);
}

void AEnemy::AI_SetCharacterMovementSpeed() { AnimInstance->SetSpeed(GetCharacterMovement()->Velocity.Size()); }

void AEnemy::AI_DisableDamageTakenRedFlash() const { if (MI_MeshBaseMaterial) MI_MeshBaseMaterial->SetScalarParameterValue("RedFlash", 0.f); }

void AEnemy::P_SetupData()
{
	P_Data->SetIsGodModeEnabled(GlobalSettingsManager->GetIsGodModeEnabled());
	MaxHp = P_Data->GetMaxHp();
	MaxHp += MaxHp * GetGameInstance()->GetSaveSlot()->GetLevelHpIncrease() * 0.01f;
	MaxHp += MaxHp * GetGameInstance()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardHealth) * 0.01f;
	CurrentHp = MaxHp;
	bIsHijacked = true;
	GetWorldTimerManager().ClearTimer(AI_PlaySpawnSoundHandle);
	bCanShoot = true;
	SetMaxWalkSpeed(P_Data->GetMaxWalkSpeed());
}

void AEnemy::P_SetupUI()
{
	P_HUD = GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD, 0);
	P_HUD->SetCharacterType(CharacterName);
	P_HUD->SetPlayerRef(this);
	P_HUD->UpdateProgressBar(EUIBar::Hijack, 0.f, P_Data->GetPossessionBarCooldown());
	P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
	P_HUD->UpdateProgressBar(EUIBar::Xp, GameInstance->GetSaveSlot()->GetXp(), GameInstance->GetSaveSlot()->GetCurrentLevelNeededXp());
	P_HUD->ConfigureUIs();
	P_UpdateAbilitiesUI();
	Weapon->P_UpdateAmmoTextHUD();
	P_SetupColorsForUI();
	GetWorldTimerManager().SetTimer(P_DetectIfPlayerPointingAtOutlineObjectsHandle, this, &AEnemy::P_DetectIfPlayerPointingAtOutlineObjects, 0.25f, true);
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
			P_HijackLogic(Cast<AEnemy>(HitResult.GetActor()));
		}
	}
}

void AEnemy::P_HijackLogic(const TObjectPtr<AEnemy>& InEnemy)
{
	if (!InEnemy || InEnemy->bIsHijacked || !InEnemy->bIsAlive) return;
	if (!InEnemy->GetIsHijackAble() || InEnemy->GetIsMutant())
	{
		AssetsManager->PlaySound2D(ESound::NotSelectable);
		if (P_HUD) P_HUD->ShowHijackUnavailable();
		return;
	}
	P_StopAllAbilities();
	DelegateManager->OnEnemyDeath.Unbind();
	Controller->Possess(InEnemy);
	InitiateDeath();
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
	if (!Controller) return;
	const FVector2D RotationVector = InValue.Get<FVector2D>();
	const FVector2D MouseSensitivity = GlobalSettingsManager->GetMouseSensitivity();
	AddControllerYawInput(RotationVector.X * MouseSensitivity.X);
	FRotator NewRotation = Camera->GetComponentRotation();
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + RotationVector.Y * MouseSensitivity.Y, -89.f, 89.f);
	Camera->SetWorldRotation(NewRotation);
}

void AEnemy::P_Reload() 
{
	if (bIsInputLocked) return;
	if (!bIsActionInMotion && Weapon->GetIsReloadEnabled() && bCanReload && !P_InputManager->IsPressed(FKey("LeftMouseButton")) && Weapon->P_CanReload()) Weapon->P_Reload();
}

void AEnemy::P_Dash()
{
	if (bIsInputLocked || bIsCrouched) return;
	if (bCanDash) P_InitiateDash();
}

void AEnemy::P_OnEnemyDeathEvents(const int32 InXp, const float InHijackTimeValue)
{
	HijackBarTimer += InHijackTimeValue;
	P_AddXp(InXp);
	for (int32 i = 0; i < Abilities.Num(); i++)
	{
		if (Abilities[i]->GetIsCooldownKill())
		{
			Abilities[i]->ReduceKillCooldown();
			if (P_HUD) P_HUD->UpdateAbilityUI(Abilities[i]);
		}
	}
}

void AEnemy::P_ToggleGodMode()
{
	GlobalSettingsManager->ToggleGodModeEnabled();
	P_Data->SetIsGodModeEnabled(GlobalSettingsManager->GetIsGodModeEnabled());
	MaxHp = P_Data->GetMaxHp();
	CurrentHp = MaxHp;
}

void AEnemy::P_UpdateAbilitiesUI() { for (int32 i = 0; i < Abilities.Num(); i++) if (bIsHijacked && !Abilities[i]->GetIsCooldownKill() && !Abilities[i]->GetIsAbilityInDuration()) P_HUD->UpdateAbilityUI(Abilities[i]); }

void AEnemy::P_StopAllAbilities() { for (int32 i = 0; i < Abilities.Num(); i++) Abilities[i]->P_StopAbility(); }

void AEnemy::P_HandleEscapeClick()
{
	if (bIsInputLocked) return;
	switch (GetWorld()->GetSubsystem<UWidgetManager>()->GetCurrentStateOfUI())
	{
		case EWidget::PauseMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI, 2)->CloseUI(); break;
		case EWidget::SettingsUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI, 2)->CloseUI(); break;
		case EWidget::VariationsUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI, 1)->CloseUI(); break;
		case EWidget::AbilitiesMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI, 1)->CloseUI(); break;
		case EWidget::UpgradeSelectionUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI, 1)->GoBack(); break;
		default: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI, 2)->OpenUI();
	}
}

void AEnemy::P_TriggerGlitchingEffect() const { GetWorld()->GetSubsystem<UPostProcessManager>()->TriggerGlitchingMaterial(1.25f); }

void AEnemy::P_MakeCameraShake(const TSubclassOf<UCameraShakeBase>& InCameraShake) const
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(InCameraShake);
}

void AEnemy::P_HandlingDamageTaken()
{
	if (GameInstance->IsPlayerTakenDamageEffectsEnabled())
	{
		P_TriggerTakeDamageEffects();
		P_HUD->UpdateReceiveDamageInv();
	}
	P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
	if (!bIsCriticallyDamaged && CurrentHp <= MaxHp * 0.3f) P_EnableCriticalDamage();
}

void AEnemy::P_TriggerTakeDamageEffects() const
{
	P_MakeCameraShake(ReceivingDamageCameraShake);
	if (UKismetMathLibrary::RandomInteger(100) >= 75)
	{
		AssetsManager->PlaySound2D(ESound::TakeDamageLayer2);
		GetWorld()->GetSubsystem<UPostProcessManager>()->TriggerGlitchingMaterial(0.25f);
	}
	else AssetsManager->PlaySound2D(ESound::TakeDamageLayer1);
}

void AEnemy::P_EnableCriticalDamage()
{
	GetWorldTimerManager().SetTimer(P_TriggerCriticalDamageIndicatorHandle, this, &AEnemy::P_TriggerCriticalDamageIndicator, 0.5f, true);
	bIsCriticallyDamaged = true;
	P_ToggleCriticallyDamagedEffects(1.f);
	P_HUD->ToggleCriticalDamageIndicator();
	AssetsManager->PlaySound2D(ESound::CriticalHealthEnter);
	AssetsManager->PlaySound2D(ESound::CriticalDamageBeep, true);
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
	AssetsManager->RemoveStoredSound(ESound::CriticalDamageBeep);
	DelegateManager->OnPlayerDeath.Broadcast();
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, 3.f, FLinearColor::Black, false, true);
	GetWorldTimerManager().SetTimer(RestartLevelHandle, this, &AEnemy::RestartLevel, 3.f, false);
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

void AEnemy::P_InUpgradeSelection()
{
	bInUpgradeSelection = true;
	P_StopAllAbilities();
}

void AEnemy::P_TryInteracting()
{
	if (bIsInputLocked) return;
	if (bPressKeyEnabled)
	{
		P_TutorialPressKeyToContinueLogic();
		return;
	}
	DelegateManager->OnInteractionTry.Broadcast();
}

void AEnemy::P_TutorialPressKeyToContinueLogic()
{
	bPressKeyEnabled = false;
	DelegateManager->OnShowPressKeyToContinue.Broadcast(false);
}

void AEnemy::P_QuickMeleeAttack()
{
	if (bIsActionInMotion || bIsInputLocked) return;
	AssetsManager->PlaySound2D(ESound::QuickMelee);
	P_MakeCameraShake(QuickMeleeCameraShake);
	bIsActionInMotion = true;
	GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::QuickMelee);
	P_ProcessMeleeAttack();
}

void AEnemy::P_ProcessMeleeAttack()
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, GetWeapon()->GetHandsSocketLocation("PlayerWeaponSocket1"), FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(QuickMeleeAttackRadiusInMeters * 100.f), Params);
	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (TObjectPtr<AActor> HitActor = Overlap.GetActor(); HitActor && HitActor != this && HitActor->ActorHasTag("AliveEnemy"))
		{
			AssetsManager->PlaySound2D(ESound::DamageTick);
			(void) DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(false);
			UGameplayStatics::ApplyDamage(HitActor, QuickMeleeAttackDamage, GetController(), this, UDamageType::StaticClass());
		}
	}
}

void AEnemy::P_SetupNavArea()
{
	NavAreaModifier = GetWorld()->SpawnActor<ANavAreaModifier>(P_NavAreaDefaultClass, GetActorLocation(), GetActorRotation());
	NavAreaModifier->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AEnemy::P_OnVariationSelection(const ECharacterType InCharacterType)
{
	if (CharacterName != InCharacterType) return;
	P_HijackLogic(AssetsManager->SpawnVariationReplacement(InCharacterType));
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI, 1)->CloseUI();
}

void AEnemy::P_ReceiveCurrentlySelectedVariation() { Variation = GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(CharacterName); }

void AEnemy::P_Jump()
{
	if (bIsInputLocked) return;
	if (CanJump()) P_InitiateJump();
}

void AEnemy::P_Stomp()
{
	if (bIsInputLocked || bIsStomping || !bCanStomp || !GetCharacterMovement()->IsFalling()) return;
	P_InitiateStomp();
}

void AEnemy::P_SetupLevelUpAppliance(const int32 InLevel)
{
	MaxHp -= MaxHp * GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(InLevel - 1) * 0.01f;
	MaxHp += MaxHp * GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(InLevel) * 0.01f;
}

void AEnemy::P_StompImpact(const FVector& InHitLocation)
{
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, InHitLocation, FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(StompRadiusInMeters * 100.f));
	for (const FOverlapResult& Result : Overlaps)
	{
		if (Result.GetActor()->ActorHasTag("AliveEnemy")) UGameplayStatics::ApplyDamage(Result.GetActor(), StompDamage, nullptr, this, UDamageType::StaticClass());
	}
}

void AEnemy::P_Death()
{
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
	Weapon->P_SetWalkSpread(GetCharacterMovement()->Velocity.Size() >= P_Data->GetMaxWalkSpeed() - 0.1f);
	Weapon->P_SetWalkingSpeed(GetCharacterMovement()->Velocity.Size());
	if (!bCanHijackOtherEnemy && !bInUpgradeSelection) P_HijackBarLogic();
	if (bCanDash) return;
	P_DashCooldownLogic();
}

void AEnemy::P_DetectIfPlayerPointingAtOutlineObjects()
{
	FHitResult HitResult;
	const FVector StartLocation = GetCamera()->GetComponentLocation();
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, StartLocation + GetCamera()->GetForwardVector() * 15000.f, ECC_Visibility))
	{
		if (const TObjectPtr<AActor> HitActor = HitResult.GetActor(); HitActor->Implements<UInteractable>())
		{
			IInteractable* Interactable = Cast<IInteractable>(HitActor);
			if (Interactable->GetIsInteractable()) Interactable->ChangeOutlineState(true);
		}
		else if (bCanHijackOtherEnemy)
		{
			const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(HitResult.GetActor());
			if (Enemy && Enemy->GetIsAlive())
			{
				if (Enemy != CurrentLookedAtEnemy)
				{
					if (CurrentLookedAtEnemy) CurrentLookedAtEnemy->SetHijackOutline(false);
					if (Enemy->GetIsHijackAble() && !Enemy->GetIsMutant()) Enemy->SetHijackOutline(true);
					else Enemy->SetUnHijackOutline(true);
					CurrentLookedAtEnemy = Enemy;
				}
				return;
			}
		}
	}
	if (CurrentLookedAtEnemy)
	{
		CurrentLookedAtEnemy->SetHijackOutline(false);
		CurrentLookedAtEnemy->SetUnHijackOutline(false);
		CurrentLookedAtEnemy = nullptr;
	}
}

void AEnemy::P_TriggerCriticalDamageIndicator() const { if (P_HUD) P_HUD->ToggleCriticalDamageIndicator(); }

void AEnemy::P_ContinuousFovChange()
{
	Camera->SetFieldOfView(Camera->FieldOfView + FovChangeTick);
	if ((FovChangeTick < 0.f && Camera->FieldOfView <= TargetFov) || (FovChangeTick > 0.f && Camera->FieldOfView >= TargetFov))
	{
		Camera->SetFieldOfView(TargetFov);
		GetWorldTimerManager().ClearTimer(P_ContinuousFovChangeHandle);
	}
}

void AEnemy::P_StopDash() const { SetMaxWalkSpeed(P_Data->GetMaxWalkSpeed()); }

void AEnemy::P_DashCooldownLogic()
{
	CurrentDashCooldown += 0.05f;
	if (CurrentDashCooldown >= P_Data->GetDashCooldown())
	{
		bCanDash = true;
		if (P_HUD) P_HUD->UpdateDashIndicator(true);
	}
}

void AEnemy::P_Crouch()
{
	if (CanCrouch()) Crouch(false);
	else UnCrouch(false);
}

void AEnemy::P_AddXp(const int32 InXp) const
{
	GameInstance->GetSaveSlot()->AddXp(GameInstance, InXp);
	if (P_HUD) P_HUD->UpdateProgressBar(EUIBar::Xp, GameInstance->GetSaveSlot()->GetXp(), GameInstance->GetSaveSlot()->GetCurrentLevelNeededXp());
}

void AEnemy::P_ApplySecretBoostHp(const int32 InPercentage)
{
	const int32 MaxHpIncrease = MaxHp * InPercentage * 0.01f;
	MaxHp += MaxHpIncrease;
	CurrentHp += MaxHpIncrease;
	if (P_HUD) P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
}

void AEnemy::TickAbilities(const float InDeltaTime)
{
	for (int32 i = 0; i < Abilities.Num(); i++) Abilities[i]->Tick(InDeltaTime);
	P_UpdateAbilitiesUI();
}

void AEnemy::SetupStimuliSource()
{
	StimuliSource = NewObject<UAIPerceptionStimuliSourceComponent>(this);
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterWithPerceptionSystem();
}

void AEnemy::ChangeXRayState(const bool bState) const
{
	GetMesh()->SetRenderCustomDepth(bState);
	GetMesh()->SetCustomDepthStencilValue(bState);
}

void AEnemy::SetupUserSettings() const
{
	const TObjectPtr<UEnhancedInputUserSettings> UserSettings = GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->GetUserSettings();
	if (!UserSettings) return;
	if (!UserSettings->IsMappingContextRegistered(PlayerMappingContext)) UserSettings->RegisterInputMappingContext(PlayerMappingContext);
}

void AEnemy::ClearTimers()
{
	GetWorldTimerManager().ClearTimer(AI_TriggerDeathVoiceLineHandle);
	GetWorldTimerManager().ClearTimer(AI_PlaySpawnSoundHandle);
	GetWorldTimerManager().ClearTimer(P_ControlLogicHandle);
	GetWorldTimerManager().ClearTimer(P_DetectIfPlayerPointingAtOutlineObjectsHandle);
	GetWorldTimerManager().ClearTimer(P_TriggerCriticalDamageIndicatorHandle);
}

void AEnemy::ProcessSpawnEffects()
{
	AssetsManager->SpawnVfx(AssetsManager->GetCharacterVfxName(CharacterName, EVfxType::Spawn), [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, GetMesh()->GetSocketLocation("SpawnSocket"));
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		if (GetMesh()->GetMaterial(i)->GetName().Contains("Emissive")) MI_MeshEmissiveMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
		else MI_MeshBaseMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
	}
	MI_MeshBaseMaterial->SetScalarParameterValue("Progress", 0.7f);
	GetWorldTimerManager().SetTimer(SpawnCondenseMaterialsProcessHandle, this, &AEnemy::SpawnCondenseMaterialsProcess, 0.05f, true);
}

void AEnemy::ProcessDamageReason(const EDamageReason InDamageReason, const float InDamage) const
{
	switch (InDamageReason)
	{
		case EDamageReason::TricksterPoisonDamage: GameInstance->GetSaveSlot()->ProgressVariationUnlock(EVariation::TricksterVariationPurple, InDamage); break;
		default: ;
	}
}

void AEnemy::ProcessKillDamageReason(const EDamageReason InDamageReason) const
{
	switch (InDamageReason)
	{
		case EDamageReason::GuardianDamage: GameInstance->GetSaveSlot()->ProgressVariationUnlock(EVariation::GuardianVariationGold, 1); break;
		case EDamageReason::RangerGrenadeDamage: GameInstance->GetSaveSlot()->ProgressVariationUnlock(EVariation::RangerVariationBlue, 1); break;
		default: ;
	}
}

void AEnemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bCanStomp = false;
	if (bIsStomping)
	{
		bIsStomping = false;
		AssetsManager->PlaySound2D(ESound::PlayerStompLand);
		AssetsManager->SpawnVfx(EVfx::PlayerStomp, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, GetSocketLocation("StompSocket"));
		GetCharacterMovement()->AirControl = 1.f;
		P_StompImpact(GetSocketLocation("StompSocket"));
		P_MakeCameraShake(StompCameraShake);
	}
}

void AEnemy::CheckIfFallOutOfWorld() { if (GetActorLocation().Z <= -1000.f) bIsHijacked ? P_Death() : InitiateDeath(); }

void AEnemy::SpawnCondenseMaterialsProcess()
{
	MeshBaseMaterialCondenseValue -= 0.015f;
	MeshEmissiveMaterialOpacityValue += 0.025f;
	MI_MeshBaseMaterial->SetScalarParameterValue("Progress", MeshBaseMaterialCondenseValue);
	MI_MeshEmissiveMaterial->SetScalarParameterValue("Opacity", MeshEmissiveMaterialOpacityValue);
	if (MeshEmissiveMaterialOpacityValue >= 0.5f) GetWorldTimerManager().ClearTimer(SpawnCondenseMaterialsProcessHandle);
}

void AEnemy::DestroyCondenseMaterialsProcess()
{
	MeshBaseMaterialCondenseValue += 0.015f;
	MeshEmissiveMaterialOpacityValue -= 0.025f;
	MI_MeshBaseMaterial->SetScalarParameterValue("Progress", MeshBaseMaterialCondenseValue);
	MI_MeshEmissiveMaterial->SetScalarParameterValue("Opacity", MeshEmissiveMaterialOpacityValue);
	if (MeshEmissiveMaterialOpacityValue <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(DestroyCondenseMaterialsProcessHandle);
		Destroy();
	}
}

void AEnemy::OpenInfoPanel() { if (P_HUD) P_HUD->ToggleInfoPanel(true); }

void AEnemy::CloseInfoPanel() { if (P_HUD) P_HUD->ToggleInfoPanel(false); }

void AEnemy::PlayEmpAffectedVfx(const float InEffectDuration)
{
	if (!EmpAffected)
	{
		AssetsManager->AttachVfx(EVfx::EmpAffected, [this]() -> bool { return GetIsAlive(); }, &EmpAffected, GetMesh(), FName("DeathSocket"), false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, EVariation::Default, {{"Loop Duration", FNiagaraAttribute(true, InEffectDuration)}});
	}
	else
	{
		if (EmpAffected->IsActive()) EmpAffected->Deactivate();
		EmpAffected->Activate();
	}
}

void AEnemy::RestartLevel() const { UGameplayStatics::OpenLevel(this, "MainMenu"); }

void AEnemy::DisableHijackViableVfx() const
{
	if (!HijackViable) return;
	HijackViable->DeactivateImmediate();
}
