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
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EUIBar.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/UI/MainWidgets/DeathScreenUI.h"
#include "PossessionGame/UI/MainWidgets/DemoEndUI.h"
#include "PossessionGame/UI/MainWidgets/HijackUI.h"
#include "PossessionGame/UI/MainWidgets/PauseMenuUI.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
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
	Sounds.Add(ESound::VectorBlastSpecialImpactUpgrade, VectorBlastSpecialImpactUpgrade);
}

bool AEnemy::GetIsHijackAble() const { return AI_Data->GetIsPossessable(); }

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
	MaxHp += GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetHpUpgradesForEnemy(CharacterName);
	CurrentHp = MaxHp;
}

void AEnemy::AI_SetupShootLogic()
{
	GetWorld()->GetTimerManager().SetTimer(AI_ShootLogicHandle, this, &AEnemy::AI_MainShootLogic, GetWeapon()->GetShootCooldown(), true);
	if (!GetWeapon()->GetIsFirstShotWasShot())
	{
		GetWeapon()->SetFirstShotWasShot();
		AI_MainShootLogic();
	}
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
	MaxHp = AI_Data->GetMaxHp() + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetHpUpgradesForEnemy(CharacterName);
	CurrentHp = MaxHp * SavedRespawnHpPercentage;
	MeshBaseMaterialCondenseValue = 0.4f;
	MeshEmissiveMaterialOpacityValue = 0.5f;
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		if (GetMesh()->GetMaterial(i)->GetName().Contains("Emissive")) MI_MeshEmissiveMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
		else MI_MeshBaseMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
	}
	if (MI_MeshBaseMaterial) MI_MeshBaseMaterial->SetScalarParameterValue("Progress", MeshBaseMaterialCondenseValue);
	if (MI_MeshEmissiveMaterial) MI_MeshEmissiveMaterial->SetScalarParameterValue("Opacity", MeshEmissiveMaterialOpacityValue);
}

void AEnemy::AI_ProcessSpawnEffects()
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

void AEnemy::AI_SetRespawnRequest(const float InHpPercentage)
{
	bAIRespawnRequested = true;
	SavedRespawnHpPercentage = InHpPercentage;
}

void AEnemy::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	AI_StopAbilitiesEvents();
	if (AnimInstance)
	{
		AnimInstance->ClearMontageWithHoldHandle();
		AnimInstance->StopAllMontages(0.f);
	}
	if (GetWeapon()->GetAnimInstance()) GetWeapon()->GetAnimInstance()->StopAllMontages(0.f);
	InHitDirection.Z = 0.f;
	GetCharacterMovement()->StopMovementImmediately();
	if (!bHitByOtherAI)
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
		ReceiveDamage(InDamage, EDamageReason::BreacherBashDamage);
	}
	LaunchCharacter(InHitDirection * (AI_Data ? AI_Data->GetBashImpulseForce() : 5000.f), true, true);
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, InEffectDuration] { ApplyStunEffect(InEffectDuration); });
	GetWorldTimerManager().SetTimer(StunEffectApplyHandle, TimerDel, 0.05f, false);
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
	ReceiveDamage(InDamage, EDamageReason::None);
	LaunchCharacter(InHitDirection * (P_Data ? P_Data->GetBashImpulseForce() : 5000.f), true, true);
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, InEffectDuration] { ApplyStunEffect(InEffectDuration); });
	GetWorldTimerManager().SetTimer(StunEffectApplyHandle, TimerDel, 0.05f, false);
	ApplyStunEffect(InEffectDuration);
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
	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(0).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Move);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(4).Action, ETriggerEvent::Started, this, &AEnemy::P_Jump);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(5).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Rotate);
		P_SetupShiftInput(EnhancedInputComponent);
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
			EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(16).Action, ETriggerEvent::Started, this, &AEnemy::ToggleTimeDilation);
		#endif
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(17).Action, ETriggerEvent::Started, this, &AEnemy::P_TryInteracting);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(18).Action, ETriggerEvent::Started, this, &AEnemy::P_QuickMeleeAttack);
		//- 19 reserved was stomp, -14 right click reserved.
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(20).Action, ETriggerEvent::Started, this, &AEnemy::OpenInfoPanel);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(20).Action, ETriggerEvent::Completed, this, &AEnemy::CloseInfoPanel);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(21).Action, ETriggerEvent::Started, this, &AEnemy::P_InitiateSlide);
	}
	if (!GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished()) SetupTutorialManager();
}

void AEnemy::PossessedBy(AController* NewController)
{
	if (!bIsHijacked)
	{
		Super::PossessedBy(NewController);
		if (Cast<APlayerController>(NewController))
		{
			bIsInvincible = true;
			GetWorldTimerManager().SetTimer(HijackInvincibilityDelayHandle, this, &AEnemy::P_TurnOffInvincibility, InvincibilityDelay);
			DisableAllVfxAfterPossession();
			bIsActionInMotion = false;
			P_TriggerGlitchingEffect();
			P_ToggleCriticallyDamagedEffects(0.f);
			AssetsManager->PlaySound2D(ESound::OnHijack);
			AssetsManager->RemoveStoredSound(ESound::GuardianSlowMotion);
			AssetsManager->RemoveStoredSound(ESound::CriticalDamageBeepFps);
			GetWorldTimerManager().SetTimer(AI_TriggerDeathVoiceLineHandle, this, &AEnemy::AI_TriggerDeathVoiceLine, 1.f, false);
			P_Setup();
			DelegateManager->RequirePlayerAnnouncementInfo.ExecuteIfBound(NSLOCTEXT("ST_Localization", "Hijacked", "Hijacked"));
			(void)DelegateManager->OnHijack.ExecuteIfBound(this);
			SetInputState(false);
		}
	}
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
		AI_PlayDeathSound();
		AI_PlayDeathExplosion();
		AI_Destroy();
		if (AI_Data) (void)DelegateManager->OnEnemyDeath.ExecuteIfBound(AI_Data->GetXp(), AI_Data->GetPossessionTimeValue());
		DelegateManager->OnEnemyDeathOccurred.Broadcast(this);
		(void)DelegateManager->UpdateGameModeEnemiesAmount.ExecuteIfBound();
		if (bAffectedByEmp && DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::TricksterAbility2, EUpgradeAttribute::Special1)) AI_TriggerEmpExplosion();
	}
	else P_Destroy();
	AI_PlayDeathElectrification();
	HandleDeactivation();
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AEnemy::DestroyThis, DestroyAfter, false);
}

void AEnemy::ReceiveDamage(const float InDamage, const EDamageReason InDamageReason)
{
	CurrentHp -= InDamage;
	if (bIsHijacked)
	{
		P_HandlingDamageTaken();
		P_ShowReceivedDamageNumber(InDamage);
	}
	else if (bIsAlive) AI_ReceivedDamage(InDamageReason, InDamage);
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

void AEnemy::ReceiveEmpEffect(const float InEffectDuration)
{
	AI_StopAbilitiesEvents();
	if (AnimInstance)
	{
		AnimInstance->ClearMontageWithHoldHandle();
		AnimInstance->StopAllMontages(0.f);
	}
	if (GetWeapon()->GetAnimInstance()) GetWeapon()->GetAnimInstance()->StopAllMontages(0.f);
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, InEffectDuration] { ApplyEmpEffect(InEffectDuration); });
	GetWorldTimerManager().SetTimer(EmpEffectApplyHandle, TimerDel, 0.05f, false);
}

void AEnemy::SetHijackOutline(const bool bState)
{
	bHijackOutlineState = bState;
	GetMesh()->SetOverlayMaterial(bState ? HijackOutlineMaterial : nullptr);
	if (bState)
	{
		if (!HijackableVfx)
		{
			if (bHijackVfxAttachRequested) return;
			bHijackVfxAttachRequested = true;
			AssetsManager->AttachVfx(EVfx::HijackViable, [this]() -> bool { return GetIsAlive() && !bIsHijacked && bHijackOutlineState; }, &HijackableVfx, GetMesh(), FName("RootSocket"));
		}
		else HijackableVfx->Activate(true);
	}
	else DisableHijackViableVfx();
}

void AEnemy::SetUnHijackOutline(const bool bState) const
{
	GetMesh()->SetOverlayMaterial(bState ? UnHijackOutlineMaterial : nullptr);
}

void AEnemy::EmpEffectEnded()
{
	UE_LOG(LogTemp, Error, TEXT("EMP EFFECT ENDED"));
	bAffectedByEmp = false;
	RemoveDisablingActionEffect();
	SetIsActionInMotion(false);
	if (AI) AI->ResumeLogic();
}

void AEnemy::ChangeOverlappingPoisonAreasInState(const bool bState)
{
	if (bState)
	{
		if (++OverlappingPoisonAreas == 1) SetActionSpeed(1.f / (bIsHijacked ? 2.f : 3.f), true);
	}
	else
	{
		if (OverlappingPoisonAreas > 0 && --OverlappingPoisonAreas == 0) SetActionSpeed(1.f, true);
	}
}

void AEnemy::SetMovementSpeedMultiplier(const float InMultiplier)
{
	MovementSpeedMultiplier = InMultiplier;
	SetMaxWalkSpeed(CurrentMoveMode);
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
	Weapon->AI_Reload();
	Weapon->AI_ResetShootCycles();
}

void AEnemy::AI_PlayDeathSound() { PlaySound(ESound::MechanicalDeath); }

void AEnemy::AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage)
{
	if (DelegateManager->RequestCurrentGameMode.IsBound() && DelegateManager->RequestCurrentGameMode.Execute() == EGameMode::HackTerminalsMode)
	{
		(void)DelegateManager->HackTerminalSubtractHp.ExecuteIfBound(InDamage + (CurrentHp <= 0.f ? CurrentHp : 0.f));
	}
	if (MI_MeshBaseMaterial) MI_MeshBaseMaterial->SetScalarParameterValue("RedFlash", 1.f);
	GetWorldTimerManager().SetTimer(AI_DamageTakenEffectHandle, this, &AEnemy::AI_DisableDamageTakenRedFlash, 0.02f, false);
	ProcessDamageReason(InDamageReason, InDamage);
	if (CurrentHp <= 0.f || !AI_HpBar || !AI_HpBarComponent) return;
    AI_HpBarComponent->SetVisibility(true);
	AI_HpBarComponent->SetComponentTickEnabled(true);
	AI_HpBar->UpdateHpBar(CurrentHp, MaxHp);
	GetWorldTimerManager().SetTimer(AI_HideHpBarHandle, this, &AEnemy::AI_HideHpBar, 2.f, false);
}

void AEnemy::AI_Destroy()
{
	AI_DestroyHpBar();
	AI_DestroyVfxs();
	if (AI)
	{
		GetWorld()->GetSubsystem<UEnemyCommunicator>()->RemoveEnemy(this);
		DelegateManager->OnXRayThroughWallsChange.RemoveAll(this);
		ChangeXRayState(false);
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
}

void AEnemy::P_SetupAbilities()
{
	TArray<int32> ChosenAbilities = GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetCharacterChosenAbilities(CharacterName).Abilities;
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
		}
	}
}

void AEnemy::P_Setup()
{
	GetMesh()->SetComponentTickEnabled(false);
	GetCharacterMovement()->SetComponentTickInterval(1.f / 60.f);
	DelegateManager->RequestCurrentCharacter.BindUObject(this, &AEnemy::GetCharacterName);
	DelegateManager->ApplyUpgradeOnSelection.BindUObject(this, &AEnemy::P_ApplyUpgradeOnSelection);
	DelegateManager->OnUpgradeSelectionOpened.BindUObject(this, &AEnemy::P_InUpgradeSelection);
	DelegateManager->RequestTriggerCameraShake.BindUObject(this, &AEnemy::P_MakeCameraShake);
	DelegateManager->OnVariationSelectionEnd.BindUObject(this, &AEnemy::P_OnVariationSelection);
	DelegateManager->OnAbilitiesMenuSelectionEnd.BindUObject(this, &AEnemy::P_OnAbilitiesMenuSelectionEnd);
	DelegateManager->OnLevelUp.AddUObject(this, &AEnemy::P_SetupLevelUpAppliance);
	DelegateManager->OnXpReceived.BindUObject(this, &AEnemy::P_AddXp);
	DelegateManager->ApplySecretBoostHealth.BindUObject(this, &AEnemy::P_ApplySecretBoostHp);
	DelegateManager->AddActionSpeedBoost.RemoveAll(this);
	Tags.Remove("AliveEnemy");
	Tags.Add("Player");
	P_ReceiveCurrentlySelectedVariation();
	AI_Destroy();
	GetMesh()->SetCollisionProfileName("Player");
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
	DelegateManager->OnEnemyDeath.BindUObject(this, &AEnemy::P_OnEnemyDeathEvents);
}

void AEnemy::P_Shoot()
{
	if (bIsInputLocked || bIsActionInMotion || !bCanShoot) return;
	if (Weapon->P_CanShoot())
	{
		if (bIsRunning) P_ToggleRun(false);
		if (bIsSliding) Weapon->GetHandsAnimInstance()->StopMontage(EHandsAnimState::Slide, 0.f);
		Weapon->P_Shoot();
	}
	else if (Weapon->P_CanReload() && Weapon->GetCurrentAmmoAmount() <= 0) Weapon->P_Reload();
}

void AEnemy::P_SetupLeftClickInput(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent)
{
	P_SetupLeftClickInputStartedState(InEnhancedInputComponent);
	P_SetupLeftClickInputTriggeredState(InEnhancedInputComponent);
	P_SetupLeftClickInputCompletedState(InEnhancedInputComponent);
}

void AEnemy::P_SetupShiftInput(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent)
{
	InEnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Started, this, &AEnemy::P_ToggleRun, true);
	InEnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Completed, this, &AEnemy::P_ToggleRun, false);
}

void AEnemy::P_SetupLeftClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent)
{
	InEnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Shoot);
}

void AEnemy::P_Destroy()
{
	DelegateManager->RequestTriggerCameraShake.Unbind();
	GetMesh()->SetHiddenInGame(false);
	StimuliSource->UnregisterFromPerceptionSystem();
	Weapon->SetToOriginalSettings();
	Weapon->DestroyThis();
	GetMesh()->SetCollisionProfileName("Enemy");
	NavAreaModifier->Destroy();
	if (bPlayerDied)
	{
		GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->SaveSlot();
		GameInstance->GetSubsystem<USaveSlotManager>()->ResetRunSaveSlot();
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
	JumpMaxCount = 50;
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
	Weapon->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::Jump);
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

void AEnemy::P_MakeCameraShake(const TSubclassOf<UCameraShakeBase>& InCameraShake) const
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(InCameraShake);
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
		ReceiveDamage(Damage, EDamageReason::None);
		return Damage;
	}
	if (RadialDamageEvent) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), Damage, GetActorLocation(), GetActorForwardVector());
	else if (PointDamageEvent)
	{
		const bool bIsCritical = PointDamageEvent->HitInfo.BoneName.IsEqual(FName("head"));
		(void)DelegateManager->ShowDamageNumber.ExecuteIfBound(bIsCritical ? CriticalDamageNumberColor : GetDamageNumberColor(), Damage, PointDamageEvent->HitInfo.Location, GetActorForwardVector());
	}
	else (void)DelegateManager->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), Damage, GetActorLocation(), GetActorForwardVector());
	ReceiveDamage(Damage, DamageCauser->Implements<UDamageReasonProvider>() ? Cast<IDamageReasonProvider>(DamageCauser)->GetDamageReason() : EDamageReason::None);
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
	GetCharacterMovement()->SetComponentTickInterval(1.f / 30.f);
	GameInstance = Cast<UPossessionGameInstance>(GetWorld()->GetGameInstance());
	DelegateManager = GameInstance->GetSubsystem<UDelegateManager>();
	GameModeBase = Cast<APossessionGameGameModeBase>(GetWorld()->GetAuthGameMode());
	GlobalSettingsManager = GameInstance->GetSubsystem<UGlobalSettingsManager>();
	DefaultCameraZ = Camera->GetRelativeLocation().Z;
	AI_SetupHpBar();
	AssetsManager = GameInstance->GetSubsystem<UPrimaryDataAssetsManager>();
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->AddEnemy(this);
	SetMesh(CharacterName);
	SetupMeshOptimization();
	SetAnimInstance();
	Tags.Add("Enemy");
	Tags.Add("AliveEnemy");
	Camera->SetActive(false);
	GetWorldTimerManager().SetTimer(CheckIfFallOutOfWorldHandle, this, &AEnemy::CheckIfFallOutOfWorld, 2.f, true);
	GetWorldTimerManager().SetTimer(AI_SetCharacterMovementSpeedHandle, this, &AEnemy::AI_SetCharacterMovementSpeed, 0.05f, true);
	GetWorldTimerManager().SetTimer(AI_PlaySpawnSoundHandle, this, &AEnemy::AI_PlaySpawnSound, 3.f + UKismetMathLibrary::RandomInteger(4), false);
	DelegateManager->OnXRayThroughWallsChange.AddUObject(this, &AEnemy::ChangeXRayState);
	DelegateManager->AddActionSpeedBoost.AddUObject(this, &AEnemy::AddAdditionalActionSpeed);
	AI_Setup();
	SetInputState(false);
	GetWorldTimerManager().SetTimer(AI_RespawnTimerHandle, this, &AEnemy::AI_SetupRespawn, 0.05f);
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
	GetWorldTimerManager().ClearTimer(P_DetectIfPlayerPointingAtOutlineObjectsHandle);
	GetWorldTimerManager().ClearTimer(P_TriggerCriticalDamageIndicatorHandle);
	GetWorldTimerManager().ClearTimer(P_DeathHandle);
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	GetWorldTimerManager().ClearTimer(CheckIfFallOutOfWorldHandle);
	GetWorldTimerManager().ClearTimer(DestroyCondenseMaterialsProcessHandle);
	GetWorldTimerManager().ClearTimer(SpawnCondenseMaterialsProcessHandle);
	Super::EndPlay(EndPlayReason);
}

void AEnemy::SetAnimInstance() { AnimInstance = Cast<UEnemyAnimationInstance>(GetMesh()->GetAnimInstance()); }

void AEnemy::SetMaxWalkSpeed(const EMoveMode InMoveMode, float InNewSpeed)
{
	SetMoveMode(InMoveMode);
	if (InNewSpeed < 0.f) InNewSpeed = ReceiveDefaultMovementSpeed();
	InNewSpeed *= ReceiveMovementSpeedModification();
	GetCharacterMovement()->MaxWalkSpeed = InNewSpeed * MovementSpeedMultiplier * ActionSpeed;
	GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.GetSafeNormal() * MovementSpeedMultiplier * InNewSpeed * ActionSpeed;
	if (AnimInstance) AnimInstance->SetMovementPlayRate(MovementSpeedMultiplier);
}

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
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
}

void AEnemy::SetMesh(const ECharacterType PrimaryDataAssetName) { GetMesh()->SetSkeletalMesh(AssetsManager->GetCharacterData(PrimaryDataAssetName).GetCharacterContainer()->CharacterMesh.Get()); }

void AEnemy::DestroyThis()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnLevelUp.RemoveAll(this);
	if (Weapon) Weapon->DestroyThis();
	GetWorldTimerManager().SetTimer(DestroyCondenseMaterialsProcessHandle, this, &AEnemy::DestroyCondenseMaterialsProcess, 0.05f, true);
}

void AEnemy::DisableAllVfxAfterPossession() { DisableHijackViableVfx(); }

void AEnemy::ClearTimers()
{
	GetWorldTimerManager().ClearTimer(AI_TriggerDeathVoiceLineHandle);
	GetWorldTimerManager().ClearTimer(AI_PlaySpawnSoundHandle);
	GetWorldTimerManager().ClearTimer(P_ControlLogicHandle);
	GetWorldTimerManager().ClearTimer(P_DetectIfPlayerPointingAtOutlineObjectsHandle);
	GetWorldTimerManager().ClearTimer(P_TriggerCriticalDamageIndicatorHandle);
	GetWorldTimerManager().ClearTimer(P_SlideEndHandle);
	GetWorldTimerManager().ClearTimer(ActionSpeedHandle);
}

void AEnemy::AI_SetupData()
{
	CurrentHp = MaxHp = AI_Data->GetMaxHp();
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
	if (!AI_HpBarComponent) return;
    AI_HpBarComponent->SetVisibility(false);
	AI_HpBarComponent->SetComponentTickEnabled(false);
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
	GetWorld()->GetTimerManager().ClearTimer(AI_ShootLogicHandle);
	AI_CancelShooting();
}

void AEnemy::AI_MainShootLogic()
{
	if (!AI || GetIsActionInMotion() || !AI->GetStateTreeData().GetIsTargetInMySight()) return;
	if (!GetAnimInstance()->IsPlaying(EEnemyAnimState::Shoot)) AI_InitiateShooting();
	if (!GetIsActionSpeedModified() || GetWeapon()->AI_CanShoot()) AI_ShootLogic();
}

void AEnemy::AI_StopAbilitiesEvents() const
{
	if (!AI) return;
	switch (AI->GetCurrentStateTreeEvent())
	{
		case EStateTreeEvent::WardenBeamShot: AI_StopAbility(0); break;
		case EStateTreeEvent::MinionBossStomp: AI_StopAbility(0); break;
		case EStateTreeEvent::MinionBossLaserEye: AI_StopAbility(1); break;
		case EStateTreeEvent::RangerLaunchGrenade: AI_StopAbility(0); break;
		case EStateTreeEvent::RangerDeployShield: AI_StopAbility(1); break;
		case EStateTreeEvent::TricksterLaunchPoisonGrenade: AI_StopAbility(0); break;
		case EStateTreeEvent::BreacherBash: AI_StopAbility(0); break;
		default: ;
	}
}

void AEnemy::AI_DisableActions()
{
/*	AI_StopAbilitiesEvents();
	if (AnimInstance)
	{
		AnimInstance->ClearMontageWithHoldHandle();
		AnimInstance->StopAllMontages(0.f);
	}
	if (GetWeapon()->GetAnimInstance()) GetWeapon()->GetAnimInstance()->StopAllMontages(0.f);
//	FTimerDelegate TimerDel;
	//TimerDel.BindLambda([this, InEffectDuration] { ApplyEmpEffect(InEffectDuration); });
//	GetWorldTimerManager().SetTimer(EmpEffectApplyHandle, TimerDel, 0.05f, false);*/
}

void AEnemy::P_SetupData()
{
	P_Data->SetIsGodModeEnabled(GlobalSettingsManager->GetIsGodModeEnabled());
	const float HpPercentage = CurrentHp / (AI_Data->GetMaxHp() + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetHpUpgradesForEnemy(CharacterName));
	MaxHp = P_Data->GetMaxHp();
	MaxHp += MaxHp * GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelHpBoost() * 0.01f;
	MaxHp += MaxHp * GameInstance->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardHealth) * 0.01f;
	CurrentHp = MaxHp * HpPercentage;
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
	P_HUD->UpdateProgressBar(EUIBar::Xp, GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetXp(), GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetCurrentLevelNeededXp());
	P_HUD->ConfigureUIs();
	P_UpdateAbilitiesUI();
	Weapon->P_UpdateAmmoTextHUD();
	P_SetupColorsForUI();
	P_HUD->PlayHijackAnim(false);
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
		P_StopAllAbilities();
		DelegateManager->OnEnemyDeath.Unbind();
		if (Controller) Controller->Possess(InEnemy);
		InitiateDeath();
		Destroy();
	}
	else
	{
		P_AddAndSetHijackPulse(1.f);
		AssetsManager->PlaySound2D(ESound::HijackInProgress);
		HijackTarget = InEnemy;
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UHijackUI>(EWidget::HijackUI)->OpenUI();
		bIsInputLocked = true;
		if (P_HUD) P_HUD->PlayHijackAnim(true);
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD)->PlayHijackInitiateAnim(true);
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
	if (!Controller || bIsInputLocked) return;
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
	if (P_HUD) P_HUD->UpdateRunIndicator(bIsRunning);
}

void AEnemy::P_OnEnemyDeathEvents(const int32 InXp, const float InHijackTimeValue)
{
//	HijackBarTimer += InHijackTimeValue * (GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished() ? 1.f : 2.f);
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

void AEnemy::P_StopAllAbilities() { for (const TPair<int32, TObjectPtr<UAbility>> Ability : Abilities) Ability.Value->P_StopAbility(); }

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
		case EWidget::ExplanationUI: break;
		default: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->OpenUI();
	}
}

void AEnemy::P_TriggerGlitchingEffect() const { GetWorld()->GetSubsystem<UPostProcessManager>()->TriggerGlitchingMaterial(1.25f); }

void AEnemy::P_MakeCameraShake(const ECameraShake InCameraShakeName) const
{
	switch (InCameraShakeName)
	{
		case ECameraShake::WardenBeam: P_MakeCameraShake(WardenBeamShake); break;
		case ECameraShake::MinionBossStomp: P_MakeCameraShake(StompCameraShake); break;
		case ECameraShake::MinionBossLaser: P_MakeCameraShake(MinionBossLaserShake);
	}
}

void AEnemy::P_HandlingDamageTaken()
{
	if (bIsImmortal && CurrentHp <= 10) CurrentHp = 10;
	P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
	if (!GameInstance->IsPlayerTakenDamageEffectsEnabled()) return;
	P_TriggerTakeDamageEffects();
	P_HUD->UpdateReceiveDamageInv();
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

void AEnemy::P_InUpgradeSelection()
{
	bInUpgradeSelection = true;
	P_StopAllAbilities();
}

void AEnemy::P_TryInteracting()
{
	if (bIsInputLocked) return;
	DelegateManager->OnInteractionTry.Broadcast();
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
	(void)DelegateManager->MeleeAttackOccured.ExecuteIfBound();
	FCollisionQueryParams OverlapParams;
	OverlapParams.AddIgnoredActor(this);
	TArray<FOverlapResult> Overlaps;
	const FVector Origin = GetWeapon()->GetHandsSocketLocation("PlayerWeaponSocket1") - GetActorForwardVector() * 30.f;
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
		const float TrueQuickMeleeDamage = QuickMeleeAttackDamage + FMath::Min(QuickMeleeAttackDamage * GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevel() * 0.1f, 80.f);
		UGameplayStatics::ApplyDamage(Actor, TrueQuickMeleeDamage, GetController(), this, UDamageType::StaticClass());
		(void)DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(false);
	}
}

void AEnemy::P_SetupNavArea()
{
	NavAreaModifier = GetWorld()->SpawnActor<ANavAreaModifier>(P_NavAreaDefaultClass, GetActorLocation(), GetActorRotation());
	NavAreaModifier->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AEnemy::P_OnVariationSelection(const ECharacterType InCharacterType)
{
	if (const TObjectPtr<AEnemy> Enemy = AssetsManager->SpawnVariationReplacement(InCharacterType, EWidget::VariationsUI)) P_HijackLogic(Enemy, true);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI)->CloseUI();
}

void AEnemy::P_OnAbilitiesMenuSelectionEnd()
{
	P_HijackLogic(AssetsManager->SpawnVariationReplacement(CharacterName, EWidget::AbilitiesMenuUI), true);
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

void AEnemy::P_SetupLevelUpAppliance()
{
	if (!LevelUpVfx) AssetsManager->AttachVfx(EVfx::LevelUpEffect, [this]() -> bool { return GetIsAlive(); }, &LevelUpVfx, GetMesh(), FName("DeathSocket"));
	else LevelUpVfx->Activate();
	MaxHp += P_Data->GetMaxHp() * GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelUpHpIncrease() * 0.01f;
	if (P_HUD) P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
}

void AEnemy::P_StompImpact(const FVector& InHitLocation)
{
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, InHitLocation, FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(StompRadiusInMeters * 100.f));
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> Actor = Result.GetActor();
		if (!Actor || !Actor->ActorHasTag("AliveEnemy")) continue;
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(Actor);
		if (GetWorld()->LineTraceSingleByChannel(Hit, InHitLocation, Actor->GetActorLocation(), ECC_Visibility, Params)) continue;
		const float TrueStompDamage = StompDamage + FMath::Min(StompDamage * GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevel() * 0.1f, 80.f);
		UGameplayStatics::ApplyDamage(Actor, TrueStompDamage, nullptr, this, UDamageType::StaticClass());
		(void)DelegateManager->OnPlayerDamageDealt.ExecuteIfBound(false);
	}
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
		if (const TObjectPtr<AActor> HitActor = HitResult.GetActor(); HitActor->Implements<UInteractable>())
		{
			IInteractable* Interactable = Cast<IInteractable>(HitActor);
			if (Interactable && Interactable->GetIsInteractable()) Interactable->ChangeOutlineState(true);
		}
		else if (bCanHijackOtherEnemy)
		{
			const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(HitResult.GetActor());
			if (Enemy && Enemy->GetIsAlive())
			{
				if (Enemy != CurrentLookedAtEnemy)
				{
					if (CurrentLookedAtEnemy) CurrentLookedAtEnemy->SetHijackOutline(false);
					if (Enemy->GetIsHijackAble()) Enemy->SetHijackOutline(true);
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

void AEnemy::P_AddXp(const int32 InXp) const
{
	if (DelegateManager->RequestIsArenaMode.IsBound() && DelegateManager->RequestIsArenaMode.Execute()) return;
	GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->AddXp(InXp);
	if (P_HUD) P_HUD->UpdateProgressBar(EUIBar::Xp, GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetXp(), GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetCurrentLevelNeededXp());
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
	LaunchCharacter(GetActorForwardVector() * SlideStrength, true, false);
	GetWorld()->GetTimerManager().SetTimer(P_SlideEndHandle, this, &AEnemy::P_EndSlide, SlideDuration, false);
	Weapon->GetHandsAnimInstance()->PlayMontageWithHold(EHandsAnimState::Slide, SlideDuration);
}

void AEnemy::P_EndSlide()
{
	Camera->SetRelativeLocation(FVector(0.f, 0.f, DefaultCameraZ));
	Weapon->GetHandsAnimInstance()->StopMontage(EHandsAnimState::Slide, 0.1f);
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

void AEnemy::P_FinishHijackProcess()
{
	GetWorldTimerManager().ClearTimer(HijackDelayHandle);
	GetWorldTimerManager().ClearTimer(HijackLockInToTargetHandle);
	P_StopAllAbilities();
	const FVector SpawnLocation = GetActorLocation();
	DelegateManager->OnEnemyDeath.Unbind();
	if (Controller) Controller->Possess(HijackTarget);
	InitiateDeath();
	Destroy();
	AssetsManager->RequestSpawnEnemy(CharacterName, SpawnLocation, CurrentHp / MaxHp);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UHijackUI>(EWidget::HijackUI)->CloseUI();
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD)->PlayHijackInitiateAnim(false);
	P_AddAndSetHijackPulse(0.f);
}

void AEnemy::P_ChangeFovWhileHijackingProcess()
{
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

void AEnemy::ProcessDamageReason(const EDamageReason InDamageReason, const float InDamage) const
{
	switch (InDamageReason)
	{
		case EDamageReason::TricksterPoisonDamage: GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->ProgressVariationUnlock(EVariation::TricksterVariationPurple, InDamage); break;
		default: ;
	}
}

void AEnemy::ProcessKillDamageReason(const EDamageReason InDamageReason) const
{
	switch (InDamageReason)
	{
		case EDamageReason::GuardianDamage:
			if (DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::GuardianAbility2, EUpgradeAttribute::Special1))
			{
				if (DelegateManager->RequestIsTimeStopUsed.IsBound() && DelegateManager->RequestIsTimeStopUsed.Execute()) GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddGuardianTimeStopSpecialStack();
			}
			GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->ProgressVariationUnlock(EVariation::GuardianVariationGold, 1); break;
		case EDamageReason::RangerGrenadeDamage: GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->ProgressVariationUnlock(EVariation::RangerVariationBlue, 1); break;
		default: ;
	}
}

void AEnemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	CurrentJumpCount = 0;
	bIsDoubleJumpOccured = false;
	if (bIsStomping)
	{
		bIsStomping = false;
		AssetsManager->PlaySound2D(ESound::PlayerStompLand);
		AssetsManager->SpawnVfx(EVfx::PlayerStomp, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, GetSocketLocation("RootSocket"));
		GetCharacterMovement()->AirControl = 1.f;
		P_StompImpact(GetSocketLocation("RootSocket"));
		P_MakeCameraShake(StompCameraShake);
	}
}

void AEnemy::CheckIfFallOutOfWorld() { if (GetActorLocation().Z <= -1000.f) bIsHijacked ? P_Death() : InitiateDeath(); }

void AEnemy::SpawnCondenseMaterialsProcess()
{
	MeshBaseMaterialCondenseValue -= 0.015f;
	MeshEmissiveMaterialOpacityValue += 0.025f;
	MI_MeshBaseMaterial->SetScalarParameterValue("Progress", MeshBaseMaterialCondenseValue);
	if (MI_MeshEmissiveMaterial) MI_MeshEmissiveMaterial->SetScalarParameterValue("Opacity", MeshEmissiveMaterialOpacityValue);
	if (MeshEmissiveMaterialOpacityValue >= 0.5f) GetWorldTimerManager().ClearTimer(SpawnCondenseMaterialsProcessHandle);
}

void AEnemy::DestroyCondenseMaterialsProcess()
{
	MeshBaseMaterialCondenseValue += 0.015f;
	MeshEmissiveMaterialOpacityValue -= 0.025f;
	MI_MeshBaseMaterial->SetScalarParameterValue("Progress", MeshBaseMaterialCondenseValue);
	if (MI_MeshEmissiveMaterial) MI_MeshEmissiveMaterial->SetScalarParameterValue("Opacity", MeshEmissiveMaterialOpacityValue);
	if (MeshEmissiveMaterialOpacityValue <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(DestroyCondenseMaterialsProcessHandle);
		Destroy();
	}
}

void AEnemy::OpenInfoPanel() { /*if (P_HUD) P_HUD->ToggleInfoPanel(true);*/ }

void AEnemy::CloseInfoPanel() { /*if (P_HUD) P_HUD->ToggleInfoPanel(false);*/ }

void AEnemy::PlayEmpAffectedVfx(const float InEffectDuration)
{
	if (!EmpAffectedVfx)
	{
		AssetsManager->AttachVfx(EVfx::EmpAffected, [this]() -> bool { return GetIsAlive(); }, &EmpAffectedVfx, GetMesh(), FName("DeathSocket"), false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, EVariation::Default, {{"Loop Duration", FNiagaraAttribute(true, InEffectDuration)}});
	}
	else EmpAffectedVfx->Activate(true);
}

void AEnemy::TogglePoisonSlowAffectedVfx(const bool bState)
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

void AEnemy::DisableHijackViableVfx() const { if (HijackableVfx) HijackableVfx->DeactivateImmediate(); }

void AEnemy::DisableVfxs() const
{
	DisableHijackViableVfx();
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

void AEnemy::SetActionSpeed(const float InSpeed, const bool bIsPoisonToggle)
{
	if (!AI_Data || !AnimInstance || !Weapon) return;
	ActionSpeed = InSpeed + AdditionalActionSpeedValue;
	bActionSpeedModified = !FMath::IsNearlyEqual(InSpeed, 1.f, 0.1f); 
	if (bIsPoisonToggle && !bIsHijacked) TogglePoisonSlowAffectedVfx(bActionSpeedModified);
	SetMaxWalkSpeed(CurrentMoveMode);
	AnimInstance->SetGlobalPlayRate(ActionSpeed);
	Weapon->SetReverseActionSpeed(1.f / ActionSpeed);
	if (Weapon->GetAnimInstance()) Weapon->GetAnimInstance()->SetGlobalPlayRate(ActionSpeed);
	else if (Weapon->GetHandsAnimInstance()) Weapon->GetHandsAnimInstance()->SetGlobalPlayRate(ActionSpeed);
}

void AEnemy::SetupMeshOptimization() const
{
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
	SetActionSpeed(ActionSpeed, false);
}

void AEnemy::ResetAdditionalActionSpeed()
{
	ActionSpeed -= AdditionalActionSpeedValue;
	AdditionalActionSpeedValue = 0.f;
	SetActionSpeed(ActionSpeed, false);
}

void AEnemy::ApplyEmpEffect(const float InEffectDuration)
{
	bAffectedByEmp = true;
	PlayEmpAffectedVfx(InEffectDuration);
	PlaySound(ESound::EmpEffectApplied, true);
	AnimInstance->PlayMontageWithHold(EEnemyAnimState::EmpEffect, InEffectDuration, true);
}

void AEnemy::ApplyStunEffect(const float InEffectDuration)
{
	if (!bIsHijacked)
	{
		PlayStunAffectedVfx(InEffectDuration);
		AnimInstance->PlayMontageWithHold(EEnemyAnimState::StunEffect, InEffectDuration, true);
	}
	else
	{
		// HANDS ANIMINSTANCE AnimInstance->PlayMontageWithHold(EEnemyAnimState::StunEffect, InEffectDuration, true);
	}
}

void AEnemy::PlayStunAffectedVfx(const float InEffectDuration)
{
	if (!StunAffectedVfx)
	{
		AssetsManager->AttachVfx(EVfx::Stunned, [this]() -> bool { return GetIsAlive(); }, &StunAffectedVfx, GetMesh(), FName("DeathSocket"), false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, EVariation::Default, {{"Loop Duration", FNiagaraAttribute(true, InEffectDuration)}});
	}
	else StunAffectedVfx->Activate(true);
}

void AEnemy::RemoveDisablingActionEffect()
{
	DisablingActionsEffectsCount--;
	DisablingActionsEffectsCount = FMath::Max(0, DisablingActionsEffectsCount);
}

void AEnemy::ApplyDisablingActionEffect()
{
	DisablingActionsEffectsCount++;
	AI_DisableActions();
}
