#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

enum class EDisablingEffect : uint8;
enum class ECameraShake : uint8;
enum class EStateTreeEvent : uint8;
struct FInputActionValue;
class UDelegateManager;
class UEnhancedPlayerInput;
class UGlobalSettingsManager;
enum class EVariation : uint8;
enum class EDamageReason : uint8;
class UNiagaraComponent;
class UPossessionGameInstance;
class APossessionGameGameModeBase;
enum class ESound : uint16;
class UAIHpBarUI;
class UWidgetComponent;
class UPrimaryDataAssetsManager;
enum class EVfx : uint16;
class ADestructible;
class ANavAreaModifier;
class UAIPerceptionStimuliSourceComponent;
class UCameraComponent;
class UInputMappingContext;
class AEnemyAIController;
class UEnemyAnimationInstance;
class UPlayerHUD;
class AWeapon;
class UAbility;
class UPlayerCharacterData;
class UAICharacterData;
enum class EAbility : uint8;
enum class ECharacterType : uint8;
enum class EMoveMode : uint8;

UCLASS(Abstract)
class POSSESSIONGAME_API AEnemy : public ACharacter
{
	GENERATED_BODY()
public:
	AEnemy();

	bool GetIsAlive() const { return bIsAlive; }
	bool IsAsyncAssetStillAllowedToBeUsed() const { return bIsAlive && DisablingActionsEffectsCount == 0; }
	bool GetIsHijacked() const { return bIsHijacked; }
	bool GetCanReload() const { return bCanReload; }
	bool GetIsActionInMotion() const { return bIsActionInMotion; }
	bool GetIsAbilityInAction() const { return bIsAbilityInAction; }
	bool GetIsStomping() const { return bIsStomping; }
	bool GetIsHijackActionViable() const { return bCanHijackOtherEnemy; }
	bool GetIsActionSpeedModified() const { return bActionSpeedModified; }
	bool GetIsHijackAble() const;
	bool CanAbilityBeExecuted(const int32 AbilityIndex) const;
	bool GetIsSliding() const { return bIsSliding; }
	bool GetIsAffectedByEmp() const { return bAffectedByEmp; }
	bool GetIsAffectedByStun() const { return bAffectedByStun; }
	bool IsActionsDisabled() const { return DisablingActionsEffectsCount > 0; }
	float GetActionSpeed() const { return ActionSpeed; }
	float GetDamageReduction() const { return DamageReduction; }
	float GetCurrentHp() const { return CurrentHp; }
	FVector GetSocketLocation(const FName& InSocketName) const { return GetMesh()->GetSocketLocation(InSocketName); }
	ECharacterType GetCharacterName() const { return CharacterName; }
	EVariation GetVariation() const { return Variation; }
	EMoveMode GetMoveMode() const { return CurrentMoveMode; }
	TObjectPtr<APossessionGameGameModeBase> GetGameModeBase() const { return GameModeBase; }
	TObjectPtr<AEnemyAIController> GetAI() const { return AI; }
	TObjectPtr<UAICharacterData> GetAIData() const { return AI_Data; }
	virtual TObjectPtr<AWeapon>& GetWeapon() { return Weapon; }
	TObjectPtr<UCameraComponent> GetCamera() { return Camera; }
	TObjectPtr<UEnemyAnimationInstance>& GetAnimInstance() { return AnimInstance; }
	TObjectPtr<UInputMappingContext>& GetMappingContext() { return PlayerMappingContext; }

	virtual void AI_InitiateShooting();
	virtual void AI_ShootLogic();
	virtual void AI_ReloadLogic();
	void AI_UseAbility(const int32 InAbilityIndex) const;
	void AI_SetMoveMode(const EMoveMode InMoveMode);
	void AI_SetShootPause();
	virtual void AI_SetCanShoot(const bool bState);
	virtual void AI_Disable();
	void AI_StopAbility(const int32 InIndex) const;
	void AI_SetupUpgradesAppliance();
	void AI_SetupShootLogic();
	virtual void AI_CancelShooting() {}
	void AI_TriggerVectorBlastSpecialImpact();
	void AI_SetupRespawn();
	void AI_ProcessSpawnEffects();
	void AI_SetRespawnRequest(const float InHpPercentage);
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection);

	void P_ApplyUpgradeOnSelection();
	void P_UseAbility(const int32 InAbilityIndex);
	void P_UpdateAmmoHUD(const int32 WeaponAmmo) const;
	void P_UpdateAmmoHUD(const int32 WeaponAmmo, const int32 MaxAmmo) const;
	void P_UpdateAmmoHUD(const int32 FirstWeaponAmmo, const int32 SecondWeaponAmmo, const int32 MaxAmmo) const;
	void P_SetFov(const float InFov);
	void P_IncreaseMovementSpeed(const float InPercentage);
	void P_ResetMovementSpeed();
	virtual void P_ReceiveStunEffect(const float InEffectDuration, const float InDamage, FVector InHitDirection);

	virtual FLinearColor GetDamageNumberColor() const { return DamageNumberColor; }

	void SetIsAbilityInAction(const bool bState) { bIsAbilityInAction = bState; }
	void SetIsActionInMotion(const bool bState);
	virtual void Tick(const float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitiateDeath();
	virtual void ReceiveDamage(const float InDamage, const EDamageReason InDamageReason);
	void PlaySound(const ESound InSoundName, const bool bIsCallbackRequired = false);
	void StopSound(const ESound InSoundName);
	virtual void ReceiveEmpEffect(const float InEffectDuration);
	void SetHijackOutline(const bool bState);
	virtual void SetUnHijackOutline(const bool bState) const;
	virtual void EmpEffectEnded();
	virtual void StunEffectEnded();
	void SetInputState(const bool bState) { bIsInputLocked = bState; }
	void SetDamageReduction(const float InValue) { DamageReduction = InValue; }
	void ChangeOverlappingPoisonAreasInState(const bool bState);
	void SetMovementSpeedMultiplier(const float InMultiplier);
	void RemoveDisablingActionEffect();
protected:
	bool bIsInputLocked = true;
	bool bCanReload = true;
	bool bIsHijacked = false;
	bool bCanShoot = false;
	bool bIsActionInMotion = false;
	bool bShootPauseActive = false;
	bool bIsAbilityInAction = false;
	bool bAffectedByEmp = false;
	bool bAffectedByStun = false;
	float MaxHp;
	float CurrentHp;
	float DamageReduction = 1.f;
	ECharacterType CharacterName;
	EVariation Variation;
	EMoveMode CurrentMoveMode;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Stomp", meta = (AllowPrivateAccess = true))
	float StompRadiusInMeters = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Stomp", meta = (AllowPrivateAccess = true))
	float StompDamage = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage Number Data", meta = (AllowPrivateAccess = true))
	FLinearColor DamageNumberColor = FLinearColor::White;
	UPROPERTY(EditDefaultsOnly, Category = "Damage Number Data", meta = (AllowPrivateAccess = true))
	FLinearColor CriticalDamageNumberColor = FLinearColor::Red;
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> AI_HpBarComponent;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_MeshBaseMaterial;
	UPROPERTY()
	TObjectPtr<UEnemyAnimationInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<AEnemyAIController> AI;
	UPROPERTY()
	TObjectPtr<UPlayerHUD> P_HUD;
	UPROPERTY()
	TObjectPtr<AWeapon> Weapon;
	UPROPERTY()
	TMap<int32, TObjectPtr<UAbility>> Abilities;
	UPROPERTY()
	TObjectPtr<UAICharacterData> AI_Data;
	UPROPERTY()
	TObjectPtr<UPlayerCharacterData> P_Data;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<UEnhancedPlayerInput> P_InputManager;
	UPROPERTY()
	TObjectPtr<UAudioComponent> VectorBlastSpecialImpactUpgrade;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	
	virtual float AI_GetShootPause();

	virtual void AI_SetupAbilities();
	virtual void AI_Setup();
	virtual void AI_SetupWeapon();
	virtual void AI_Shoot();
	void AI_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName);
	virtual void AI_SetController();
	virtual void AI_PlayShootMontage();
	void AI_Reload() const;
	virtual void AI_PlayDeathSound();
	virtual void AI_PlayDeathExplosion() {}
	virtual void AI_PlayDeathElectrification() {}
	virtual void AI_TriggerDeathVoiceLine() {}
	virtual void AI_PlaySpawnSound() {}
	virtual void AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage);
	virtual void AI_Destroy();
	virtual void AI_DestroyVfxs();
	virtual void AI_DisableActions(const float InEffectDuration, const EDisablingEffect InEffectName);

	virtual void P_SetupAbilities();
	virtual void P_Setup();
	virtual void P_Shoot();
	virtual void P_SetupLeftClickInput(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent);
	virtual void P_SetupRightClickInput(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent) {}
	virtual void P_SetupShiftInput(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent);
	virtual void P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent) {}
	virtual void P_SetupLeftClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent);
	virtual void P_SetupLeftClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent) {}
	virtual void P_Destroy();
	void P_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName);
	void P_SetupCharacterMovement();
	virtual void P_SetupColorsForUI() {}
	virtual void P_Move(const FInputActionValue& Value);
	virtual void P_InitiateJump();
	virtual void P_InitiateStomp();
	virtual void P_ShowReceivedDamageNumber(const float InDamage);
	void P_MakeCameraShake(const TSubclassOf<UCameraShakeBase>& InCameraShake) const;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	float ReceiveDefaultMovementSpeed() const;
	virtual float ReceiveMovementSpeedModification() const { return 1.f; }

	void SetCurrentHp(const float NewCurrentHp) { CurrentHp = NewCurrentHp; }
	void SetMaxHp(const float NewMaxHp) { MaxHp = NewMaxHp; }
	virtual void SetMoveMode(const EMoveMode InMoveMode) { CurrentMoveMode = InMoveMode; }
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetAnimInstance();
	void SetMaxWalkSpeed(const EMoveMode InMoveMode, float InNewSpeed = -1.f);
	void AttachDestructible(const TObjectPtr<ADestructible>& Destructible, const FName SocketName) const;
	virtual void HandleDeactivation();
	virtual void SetMesh(const ECharacterType PrimaryDataAssetName);
	virtual void DestroyThis();
	virtual void DisableAllVfxAfterPossession();
	virtual void ClearTimers();
	virtual void AllDisablingActionsFinished();
private:
	bool bInUpgradeSelection = false;
	bool bCanHijackOtherEnemy = false;
	bool bIsAlive = true;
	bool bEnableUnloadLevel = false;
	bool bIsCriticallyDamaged = false;
	bool bIsAIBarCreated = false;
	bool bPlayerDied = false;
	bool bIsStomping = false;
	bool bCanRun = true;
	bool bIsRunning = false;
	bool bHijackOutlineState = false;
	bool bIsSliding = false;
	bool bIsDoubleJumpOccured = false;
	bool bIsImmortal = false;
	bool bActionSpeedModified = false;
	bool bIsPoisonSlowAffected = false;
	bool bHijackVfxAttachRequested = false;
	bool bPoisonSlowVfxAttachRequested = false;
	bool bIsInvincible = false;
	bool bAIRespawnRequested = false;
	float HijackBarTimer = 0.f;
	float MeshBaseMaterialCondenseValue = 0.7f;
	float MeshEmissiveMaterialOpacityValue = 0.f;
	float FovChangeTick;
	float TargetFov;
	float DefaultCameraZ;
	float ActionSpeed = 1.f;
	float MovementSpeedMultiplier = 1.f;
	float AdditionalActionSpeedValue = 0.f;
	float SavedRespawnHpPercentage = 1.f;
	int32 P_FirstSlotAbilityIndex = 0;
	int32 P_SecondSlotAbilityIndex = 1;
	int32 OverlappingPoisonAreas = 0;
	int32 DisablingActionsEffectsCount = 0;
	int32 CurrentJumpCount = 0;
	FTimerHandle AI_ClearShootPauseHandle;
	FTimerHandle AI_TriggerDeathVoiceLineHandle;
	FTimerHandle AI_SetCharacterMovementSpeedHandle;
	FTimerHandle AI_PlaySpawnSoundHandle;
	FTimerHandle AI_HideHpBarHandle;
	FTimerHandle AI_DamageTakenEffectHandle;
	FTimerHandle AI_ShootLogicHandle;
	FTimerHandle AI_RespawnTimerHandle;
	FTimerHandle P_DetectIfPlayerPointingAtOutlineObjectsHandle;
	FTimerHandle P_TriggerCriticalDamageIndicatorHandle;
	FTimerHandle P_DeathHandle;
	FTimerHandle P_ContinuousFovChangeHandle;
	FTimerHandle P_ControlLogicHandle;
	FTimerHandle P_SlideEndHandle;
	FTimerHandle DestroyHandle;
	FTimerHandle CheckIfFallOutOfWorldHandle;
	FTimerHandle DestroyCondenseMaterialsProcessHandle;
	FTimerHandle SpawnCondenseMaterialsProcessHandle;
	FTimerHandle ActionSpeedHandle;
	FTimerHandle EmpEffectEndHandle;
	FTimerHandle StunEffectEndHandle;
	FTimerHandle HijackDelayHandle;
	FTimerHandle HijackFovChangeHandle;
	FTimerHandle HijackLockInToTargetHandle;
	FTimerHandle HijackInvincibilityDelayHandle;
	FTimerHandle RecoverFromDisablingEffectHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Explosion Radius From Emp Upgrade", meta = (AllowPrivateAccess = true))
	float ExplosionRadiusFromEmpUpgradeInMetres = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Hijack", meta = (AllowPrivateAccess = true))
	float HijackDelay = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Hijack", meta = (AllowPrivateAccess = true))
	float InvincibilityDelay = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Destroy After Seconds", meta = (AllowPrivateAccess = true))
	float DestroyAfter = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Quick Melee Attack", meta = (AllowPrivateAccess = true))
	float QuickMeleeAttackRadiusInMeters = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Quick Melee Attack", meta = (AllowPrivateAccess = true))
	float QuickMeleeAttackDamage = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	bool bCanStomp = true;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float GravityScale = 3.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float JumpVelocity = 1150.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float AirControl = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float MaxAcceleration = 20000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float BrakingFrictionFactor = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float GroundFriction = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	int32 MaxJumpAmount = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Slide", meta = (AllowPrivateAccess = true))
	bool bCanSlide = true;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Slide", meta = (EditConditionHides = "bCanSlide", AllowPrivateAccess = true))
	float SlideBrakingFrictionFactor = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Slide", meta = (EditConditionHides = "bCanSlide", AllowPrivateAccess = true))
	float SlideGroundFriction = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Slide", meta = (EditConditionHides = "bCanSlide", AllowPrivateAccess = true))
	float SlideStrength = 2500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Slide", meta = (EditConditionHides = "bCanSlide", AllowPrivateAccess = true))
	float SlideDuration = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Slide", meta = (EditConditionHides = "bCanSlide", AllowPrivateAccess = true))
	float SlideHeight = 39.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Slide", meta = (EditConditionHides = "bCanSlide", AllowPrivateAccess = true))
	float SlideCameraZ = 0.f;
	UPROPERTY(EditAnywhere, Category = "Nav", meta = (AllowPrivateAccess = true))
	TSubclassOf<ANavAreaModifier> P_NavAreaDefaultClass;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> StompCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> WardenBeamShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> MinionBossLaserShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> ReceivingDamageCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> QuickMeleeCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> HijackOutlineMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> UnHijackOutlineMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_HijackPulse;
	UPROPERTY()
	TObjectPtr<UAIHpBarUI> AI_HpBar;
	UPROPERTY()
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;
	UPROPERTY()
	TObjectPtr<ANavAreaModifier> NavAreaModifier;
	UPROPERTY()
	TObjectPtr<APossessionGameGameModeBase> GameModeBase;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_MeshEmissiveMaterial;
	UPROPERTY()
	TObjectPtr<UPossessionGameInstance> GameInstance;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> EmpAffectedVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> StunAffectedVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> PoisonSlowAffectedVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> HijackableVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LevelUpVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> RunVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VectorBlastSpecialImpactVfx;
	UPROPERTY()
	TObjectPtr<AEnemy> CurrentLookedAtEnemy;
	UPROPERTY()
	TObjectPtr<UGlobalSettingsManager> GlobalSettingsManager;
	UPROPERTY()
	TObjectPtr<AEnemy> HijackTarget;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_HijackPulse;

	void AI_SetupData();
	void AI_StopAllAbilities();
	void AI_SetupHpBar();
	void AI_DestroyHpBar();
	void AI_TriggerEmpExplosion();
	void AI_HideHpBar() const;
	void AI_ClearShootPause();
	void AI_SetCharacterMovementSpeed() const;
	void AI_DisableDamageTakenRedFlash() const;
	void AI_FinishShootLogicByEvent(const EStateTreeEvent InEvent);
	void AI_MainShootLogic();
	void AI_StopAbilitiesEvents() const;
	void AI_TimeDilationChange(const float InDilation) const;

	int32 P_GetFirstSlotAbilityIndex() const { return P_FirstSlotAbilityIndex; }
	int32 P_GetSecondSlotAbilityIndex() const { return P_SecondSlotAbilityIndex; }
	
	void P_SetupData();
	void P_SetupUI();
	void P_HijackEnemy();
	void P_HijackLogic(const TObjectPtr<AEnemy>& InEnemy, const bool bInstantHijack);
	void P_HijackBarLogic();
	void P_Rotate(const FInputActionValue& InValue);
	void P_Reload();
	void P_ToggleRun(const bool bState);
	void P_OnEnemyDeathEvents(const int32 InXp, const float InHijackTimeValue);
	void P_ToggleGodMode();
	void P_UpdateAbilitiesUI();
	void P_StopAllAbilities();
	void P_HandleEscapeClick();
	void P_TriggerGlitchingEffect() const;
	void P_MakeCameraShake(const ECameraShake InCameraShakeName) const;
	void P_HandlingDamageTaken();
	void P_TriggerTakeDamageEffects() const;
	void P_EnableCriticalDamage();
	void P_ToggleCriticallyDamagedEffects(const float InValue) const;
	void P_InitiateDeathEvents();
	void P_ConfigurePostProcessEffectsAfterDeath() const;
	void P_InUpgradeSelection();
	void P_TryInteracting();
	void P_QuickMeleeAttack();
	void P_ProcessMeleeAttack();
	void P_SetupNavArea();
	void P_OnVariationSelection(const ECharacterType InCharacterType);
	void P_OnAbilitiesMenuSelectionEnd();
	void P_ReceiveCurrentlySelectedVariation();
	void P_Jump();
	void P_Stomp();
	void P_SetupLevelUpAppliance();
	void P_StompImpact(const FVector& InHitLocation);
	void P_Death();
	void P_ControlLogic();
	void P_DetectIfPlayerPointingAtOutlineObjects();
	void P_TriggerCriticalDamageIndicator() const;
	void P_ContinuousFovChange();
	void P_AddXp(const int32 InXp) const;
	void P_ApplySecretBoostHp(const int32 InPercentage);
	void P_InitiateSlide();
	void P_EndSlide();
	void P_SetupSuperJump();
	void P_FinishHijackProcess();
	void P_ChangeFovWhileHijackingProcess();
	void P_AddAndSetHijackPulse(const float InValue);
	void P_LockInToTargetLogic() const;
	void P_TurnOffInvincibility();

	void TickAbilities(const float InDeltaTime);
	void SetupStimuliSource();
	void ChangeXRayState(const bool bState) const;
	void SetupUserSettings() const;
	void ProcessDamageReason(const EDamageReason InDamageReason, const float InDamage) const;
	void ProcessKillDamageReason(const EDamageReason InDamageReason) const;
	virtual void Landed(const FHitResult& Hit) override;
	void CheckIfFallOutOfWorld();
	void SpawnCondenseMaterialsProcess();
	void DestroyCondenseMaterialsProcess();
	void OpenInfoPanel();
	void CloseInfoPanel();
	void PlayEmpAffectedVfx(const float InEffectDuration);
	void TogglePoisonSlowAffectedVfx(const bool bState);
	void DisableHijackViableVfx() const;
	void DisableVfxs() const;
	void SetupTutorialManager();
	void ToggleTimeDilation();
	void SetActionSpeed(const float InSpeed, const bool bIsPoisonToggle);
	void SetupMeshOptimization() const;
	void AddAdditionalActionSpeed(const float InValue, const float InDuration);
	void ResetAdditionalActionSpeed();
	void ApplyEmpEffect(const float InEffectDuration);
	void ApplyStunEffect(const float InEffectDuration);
	void PlayStunAffectedVfx(const float InEffectDuration);
	void ApplyDisablingActionEffect(const float InEffectDuration, const EDisablingEffect InEffectName);
	void RecoverFromDisablingEffect();
};