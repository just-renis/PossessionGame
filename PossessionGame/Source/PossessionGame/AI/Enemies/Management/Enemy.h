#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

enum class EStateTreeEvent : uint8;
struct FInputActionValue;
class UDelegateManager;
class UEnhancedPlayerInput;
enum class EWidget : uint8;
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
	bool IsAsyncAssetStillAllowedToBeUsed() const { return bIsAlive && !bAffectedByEmp; }
	bool GetIsHijacked() const { return bIsHijacked; }
	bool GetCanReload() const { return bCanReload; }
	bool GetIsActionInMotion() const { return bIsActionInMotion; }
	bool GetIsMutant() const { return bIsMutant; }
	bool GetIsAbilityInAction() const { return bIsAbilityInAction; }
	bool GetIsStomping() const { return bIsStomping; }
	bool GetIsHijackActionViable() const { return bCanHijackOtherEnemy; }
	bool GetIsActionSpeedModified() const { return bActionSpeedModified; }
	bool GetIsHijackAble() const;
	bool CanAbilityBeExecuted(const int32 AbilityIndex);
	float GetActionSpeed() const { return ActionSpeed; }
	float GetDamageReduction() const { return DamageReduction; }
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
	void AI_UseAbility(const int32 AbilityIndex);
	void AI_SetMoveMode(const EMoveMode InMoveMode);
	void AI_SetShootPause();
	virtual void AI_SetCanShoot(const bool bState);
	virtual void AI_Disable();
	void AI_StopAbility(const int32 InIndex);
	void AI_TryEvolving();
	void AI_SetupUpgradesAppliance();
	//void AI_SetupShootLogic();
	virtual void AI_CancelShooting() {}

	virtual FLinearColor GetDamageNumberColor() const { return DamageNumberColor; }

	void P_ApplyUpgradeOnSelection();
	void P_UseAbility(const int32 AbilityIndex);
	void P_UpdateAmmoHUD(const int32 WeaponAmmo) const;
	void P_UpdateAmmoHUD(const int32 WeaponAmmo, const int32 MaxAmmo) const;
	void P_UpdateAmmoHUD(const int32 FirstWeaponAmmo, const int32 SecondWeaponAmmo, const int32 MaxAmmo) const;
	void P_SetFov(const float InFov);
	void P_IncreaseMovementSpeed(const float InPercentage);
	void P_ResetMovementSpeed();

	void SetIsAbilityInAction(const bool bState) { bIsAbilityInAction = bState; }
	void SetIsActionInMotion(const bool bState);
	virtual void Tick(const float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitiateDeath();
	virtual void ReceiveDamage(const float InDamage, const EDamageReason InDamageReason);
	void PlaySound(const ESound InSoundName, const bool bIsCallbackRequired = false);
	void StopSound(const ESound InSoundName);
	virtual void SetEmpEffect(const float InEffectDuration);
	void SetHijackOutline(const bool bState);
	virtual void SetUnHijackOutline(const bool bState) const;
	virtual void EmpEffectEnded();
	void SetInputState(const bool bState) { bIsInputLocked = bState; }
	void SetDamageReduction(const float InValue) { DamageReduction = InValue; }
	void ChangeOverlappingPoisonAreasInState(const bool bState);
	void SetMovementSpeedMultiplier(const float InMultiplier);
protected:
	bool bIsInputLocked = true;
	bool bCanReload = true;
	bool bIsHijacked = false;
	bool bCanShoot = false;
	bool bIsActionInMotion = false;
	bool bShootPauseActive = false;
	bool bIsAbilityInAction = false;
	bool bAffectedByEmp = false;
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
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> StompCameraShake;
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
	TArray<TObjectPtr<UAbility>> Abilities;
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
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

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

	virtual void P_SetupAbilities();
	virtual void P_Setup();
	virtual void P_Shoot();
	virtual void P_SetupLeftClickInput(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) {}
	virtual void P_SetupLeftClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) {}
	virtual void P_Destroy();
	void P_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName);
	void P_SetupCharacterMovement() const;
	virtual void P_SetupColorsForUI() {}
	virtual void P_Move(const FInputActionValue& Value);
	virtual void P_InitiateJump();
	virtual void P_InitiateStomp();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	float ReceiveDefaultMovementSpeed() const;
	virtual float ReceiveMovementSpeedModification() const { return 1.f; }

	void SetCurrentHp(const float NewCurrentHp) { CurrentHp = NewCurrentHp; }
	void SetMaxHp(const float NewMaxHp) { MaxHp = NewMaxHp; }
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetAnimInstance();
	void SetMaxWalkSpeed(const EMoveMode InMoveMode, float InNewSpeed = -1.f);
	void AttachDestructible(const TObjectPtr<ADestructible>& Destructible, const FName SocketName) const;
	virtual void HandleDeactivation();
	virtual void SetMesh(const ECharacterType PrimaryDataAssetName);
	virtual void DestroyAI();
	virtual void DestroyThis();
	virtual void DisableAllVfxAfterPossession();
	virtual void SetMoveMode(const EMoveMode InMoveMode) { CurrentMoveMode = InMoveMode; }
	virtual void DestroyAIVfxs();
private:
	bool bInUpgradeSelection = false;
	bool bCanHijackOtherEnemy = false;
	bool bIsAlive = true;
	bool bEnableUnloadLevel = false;
	bool bIsCriticallyDamaged = false;
	bool bIsAIBarCreated = false;
	bool bPlayerDied = false;
	bool bIsMutant = false;
	bool bIsStomping = false;
	bool bCanRun = true;
	bool bIsRunning = false;
	bool bHijackOutlineState = false;
	bool bIsSliding = false;
	bool bIsDoubleJumpOccured = false;
	bool bIsImmortal = false;
	bool bActionSpeedModified = false;
	float HijackBarTimer = 0.f;
	float MeshBaseMaterialCondenseValue = 0.7f;
	float MeshEmissiveMaterialOpacityValue = 0.f;
	float FovChangeTick;
	float TargetFov;
	float DefaultCameraZ;
	float ActionSpeed = 1.f;
	float MovementSpeedMultiplier = 1.f;
	int32 OverlappingPoisonAreas = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Destroy After Seconds", meta = (AllowPrivateAccess = true))
	float DestroyAfter = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Quick Melee Attack", meta = (AllowPrivateAccess = true))
	float QuickMeleeAttackRadiusInMeters = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Quick Melee Attack", meta = (AllowPrivateAccess = true))
	float QuickMeleeAttackDamage = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Mutant", meta = (AllowPrivateAccess = true))
	float MutantExplosionDamage = 50.f;
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
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Crouch", meta = (AllowPrivateAccess = true))
	bool bCanCrouch = true;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Crouch", meta = (EditConditionHides = "bCanCrouch", AllowPrivateAccess = true))
	float CrouchHalfHeight = 65.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Crouch", meta = (EditConditionHides = "bCanCrouch", AllowPrivateAccess = true))
	float MaxWalkSpeedCrouched = 350.f;
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
	TSubclassOf<UCameraShakeBase> ReceivingDamageCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> QuickMeleeCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> HijackOutlineMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> UnHijackOutlineMaterial;
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
	TObjectPtr<UNiagaraComponent> Aura;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> EmpAffected;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> PoisonSlowAffected;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> HijackViable;
	UPROPERTY()
	TObjectPtr<AEnemy> CurrentLookedAtEnemy;
	UPROPERTY()
	TObjectPtr<UGlobalSettingsManager> GlobalSettingsManager;
	UPROPERTY()
	FTimerHandle AI_ClearShootPauseHandle;
	UPROPERTY()
	FTimerHandle AI_TriggerDeathVoiceLineHandle;
	UPROPERTY()
	FTimerHandle AI_SetCharacterMovementSpeedHandle;
	UPROPERTY()
	FTimerHandle AI_PlaySpawnSoundHandle;
	UPROPERTY()
	FTimerHandle AI_HideHpBarHandle;
	UPROPERTY()
	FTimerHandle AI_DamageTakenEffectHandle;
	UPROPERTY()
	FTimerHandle AI_ShootLogicHandle;
	UPROPERTY()
	FTimerHandle P_DetectIfPlayerPointingAtOutlineObjectsHandle;
	UPROPERTY()
	FTimerHandle P_TriggerCriticalDamageIndicatorHandle;
	UPROPERTY()
	FTimerHandle P_DeathHandle;
	UPROPERTY()
	FTimerHandle P_ContinuousFovChangeHandle;
	UPROPERTY()
	FTimerHandle P_ControlLogicHandle;
	UPROPERTY()
	FTimerHandle P_SlideEndHandle;
	UPROPERTY()
	FTimerHandle DestroyHandle;
	UPROPERTY()
	FTimerHandle CheckIfFallOutOfWorldHandle;
	UPROPERTY()
	FTimerHandle DestroyCondenseMaterialsProcessHandle;
	UPROPERTY()
	FTimerHandle SpawnCondenseMaterialsProcessHandle;
	UPROPERTY()
	FTimerHandle RestartLevelHandle;

	void AI_SetupData();
	void AI_StopAllAbilities();
	void AI_SetupHpBar();
	void AI_DestroyHpBar();
	void AI_TriggerMutantExplosion();
	void AI_HideHpBar() const;
	void AI_ClearShootPause();
	void AI_SetCharacterMovementSpeed() const;
	void AI_DisableDamageTakenRedFlash() const;
	//void AI_FinishShootLogicByEvent(const EStateTreeEvent InEvent);
	//void AI_MainShootLogic();
	
	void P_SetupData();
	void P_SetupUI();
	void P_HijackEnemy();
	void P_HijackLogic(const TObjectPtr<AEnemy>& InEnemy);
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
	void P_MakeCameraShake(const TSubclassOf<UCameraShakeBase>& InCameraShake) const;
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
	void P_SetupLevelUpAppliance(const int32 InLevel);
	void P_StompImpact(const FVector& InHitLocation);
	void P_Death();
	void P_ControlLogic();
	void P_DetectIfPlayerPointingAtOutlineObjects();
	void P_TriggerCriticalDamageIndicator() const;
	void P_ContinuousFovChange();
	void P_Crouch();
	void P_AddXp(const int32 InXp) const;
	void P_ApplySecretBoostHp(const int32 InPercentage);
	void P_InitiateSlide();
	void P_EndSlide();
	void P_SetupSuperJump();

	virtual bool CanJumpInternal_Implementation() const override;

	void TickAbilities(const float InDeltaTime);
	void SetupStimuliSource();
	void ChangeXRayState(const bool bState) const;
	void SetupUserSettings() const;
	void ClearTimers();
	void ProcessSpawnEffects();
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
	void RestartLevel() const;
	void DisableHijackViableVfx() const;
	void SetupTutorialManager();
	void ToggleTimeDilation();
	void SetActionSpeed(const float InSpeed);
	void SetupMeshOptimization() const;
};