#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputTriggers.h"
#include "EnhancedInputComponent.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "Enemy.generated.h"

class AAIHpBar;
enum class EVariation : uint8;
enum class EDamageReason : uint8;
class UNiagaraComponent;
class UPossessionGameInstance;
class APossessionGameGameModeBase;
enum class ESound : uint16;
class UAIHpBarUI;
class UWidgetComponent;
class UPrimaryDataAssetsManager;
enum class EVfx : uint8;
class ADestructible;
enum class EMoveMode : uint8;
class ANavAreaModifier;
class UAIPerceptionStimuliSourceComponent;
class UAbilityData;
class UInputAction;
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

UCLASS(Abstract)
class POSSESSIONGAME_API AEnemy : public ACharacter
{
	GENERATED_BODY()
public:
	AEnemy();

	bool GetIsAlive() const { return bIsAlive; }
	bool IsAsyncAssetStillAllowedToBeUsed() const { return bIsAlive && !bAffectedByEmp; }
	bool GetIsPossessed() const { return bIsPossessed; }
	bool GetCanReload() const { return bCanReload; }
	bool GetIsPossessable() const;
	bool GetIsActionInMotion() const { return bIsActionInMotion; }
	bool CanAbilityBeExecuted(const int32 AbilityIndex) { return Abilities[AbilityIndex]->CanExecute(); }
	bool GetIsElite() const { return bIsElite; }
	bool GetIsMutant() const { return bIsMutant; }
	float GetSkeletalMeshHeight() const;
	FVector GetSocketLocation(const FName& InSocketName) const { return GetMesh()->GetSocketLocation(InSocketName); }
	ECharacterType GetCharacterName() const { return CharacterName; }
	EVariation GetVariation() const { return Variation; }
	TObjectPtr<APossessionGameGameModeBase> GetGameModeBase() const { return GameModeBase; }
	TObjectPtr<AEnemyAIController> GetAI() const { return AI; }
	TObjectPtr<UAICharacterData> GetAIData() const { return AI_Data; }
	virtual TObjectPtr<AWeapon>& GetWeapon() { return Weapon; }
	TObjectPtr<UCameraComponent> GetCamera() { return Camera; }
	TObjectPtr<UEnemyAnimationInstance>& GetAnimInstance() { return AnimInstance; }
	TObjectPtr<UPossessionGameInstance>& GetGameInstance() { return GameInstance; }

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
	virtual void AI_CancelShooting() {}

	virtual void P_ApplyUpgradeOnSelection(const EUpgrade InUpgradeName) { bInUpgradeSelection = false; }
	void P_UseAbility(const int32 AbilityIndex);
	void P_UpdateAmmoHUD(const int32 WeaponAmmo) const;
	void P_UpdateAmmoHUD(const int32 WeaponAmmo, const int32 MaxAmmo) const;
	void P_UpdateAmmoHUD(const int32 FirstWeaponAmmo, const int32 SecondWeaponAmmo, const int32 MaxAmmo) const;
	void P_StopTime(); // NOT USED ANYWHERE YET.
	void P_SetFov(const float InFov);
	void P_IncreaseMovementSpeed(const float InPercentage) const;
	void P_ResetMovementSpeed() const;

	void SetAbilityInAction(const bool bState) { bAbilityInAction = bState; }
	void SetActionInMotion(const bool bState);
	virtual void Tick(const float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitiateDeath();
	virtual void ReceiveDamage(const float InDamage, const EDamageReason InDamageReason);
	void PlaySound(const ESound InSoundName, const bool bIsCallbackRequired = false);
	void StopSound(const ESound InSoundName);
	virtual void SetEmpEffect(const float InEffectDuration);
	void SetPossessionOutline(const bool bState) const;
	void SetUnPossessionOutline(const bool bState) const;
	void SetPossessionOutlineTimer();
	void SetUnPossessionOutlineTimer();
	virtual void EmpEffectEnded();
	void SetInputState(const bool bState) { bIsInputLocked = bState; }
	void SetDamageReduction(const float InValue) { DamageReduction = InValue; }
protected:
	bool bIsInputLocked = true;
	bool bCanReload = true;
	bool bIsPossessed = false;
	bool bCanShoot = false;
	bool bIsActionInMotion = false;
	bool bShootPauseActive = false;
	bool bAbilityInAction = false;
	bool bAffectedByEmp = false;
	float MaxHp;
	float CurrentHp;
	ECharacterType CharacterName;
	EVariation Variation;
	EMoveMode CurrentMoveMode = EMoveMode::Walk;
	FVector2D MouseSensitivity = FVector2D(1.f, 1.f);
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Stomp", meta = (AllowPrivateAccess = true))
	float StompRadiusInMeters = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Stomp", meta = (AllowPrivateAccess = true))
	float StompDamage = 50.f;
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> StompCameraShake;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> Camera;
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

	virtual void P_SetupAbilities();
	virtual void P_Setup();
	virtual void P_Shoot(const bool bFirstWeapon);
	virtual void P_SetupLeftClickInput(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) {}
	virtual void P_SetupLeftClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) {}
	virtual void P_SetupRightClickInput(const TObjectPtr<UEnhancedInputComponent> RightClickInputComponent);
	virtual void P_SetupRightClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_SetupRightClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_SetupRightClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_Destroy();
	void P_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName);
	void P_SetupCharacterMovement();
	virtual void P_SetupColorsForUI() {}
	virtual void P_Move(const FInputActionValue& Value);
	virtual void P_InitiateDash();
	virtual void P_InitiateJump();
	virtual void P_InitiateStomp();

	void SetCurrentHp(const float NewCurrentHp) { CurrentHp = NewCurrentHp; }
	void SetMaxHp(const float NewMaxHp) { MaxHp = NewMaxHp; }
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetAnimInstance();
	void SetMaxWalkSpeed(const float Speed) const;
	void SetInitialWalkSpeed(const bool bState) const;
	void SetupAbilities(const TMap<EAbility, TSoftObjectPtr<UAbilityData>>& AbilityData);
	void AttachDestructible(const TObjectPtr<ADestructible>& Destructible, const FName SocketName) const;
	virtual void HandleDeactivation();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetMesh(const ECharacterType PrimaryDataAssetName);
	virtual void DestroyAI();
	virtual void DestroyThis();
private:
	bool bPressKeyEnabled = false;
	bool bInUpgradeSelection = false;
	bool bCanPossessOtherEnemy = false;
	bool bIsAlive = true;
	bool bEnableUnloadLevel = false;
	bool bIsCriticallyDamaged = false;
	bool bIsAIBarCreated = false;
	bool bPlayerDied = false;
	bool bIsElite = false;
	bool bIsMutant = false;
	bool bIsStomping = false;
	bool bCanStomp = false;
	bool bCanDash = true;
	float PossessionBarTimer = 0.f;
	float MeshBaseMaterialCondenseValue = 0.7f;
	float MeshEmissiveMaterialOpacityValue = 0.f;
	float FovChangeTick;
	float TargetFov;
	float CurrentDashCooldown;
	float DamageReduction = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Destroy After Seconds", meta = (AllowPrivateAccess = true))
	float DestroyAfter = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Quick Melee Attack", meta = (AllowPrivateAccess = true))
	float QuickMeleeAttackRadiusInMeters = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Quick Melee Attack", meta = (AllowPrivateAccess = true))
	float QuickMeleeAttackDamage = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Mutant", meta = (AllowPrivateAccess = true))
	float MutantExplosionDamage = 50.f;
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
	int32 MaxJumpAmount = 2;
	UPROPERTY(EditAnywhere, Category = "Nav", meta = (AllowPrivateAccess = true))
	TSubclassOf<ANavAreaModifier> P_NavAreaDefaultClass;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> ReceivingDamageCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> QuickMeleeCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> PossessionOutlineMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> UnPossessionOutlineMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> AI_HpBarComponent;
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
	FTimerHandle AI_ClearShootPauseHandle;
	UPROPERTY()
	FTimerHandle P_ContinuousFovChangeHandle;
	UPROPERTY()
	FTimerHandle AI_TriggerDeathVoiceLineHandle;
	UPROPERTY()
	FTimerHandle DestroyHandle;
	UPROPERTY()
	FTimerHandle DisablePossessionOutlineHandle;
	UPROPERTY()
	FTimerHandle DisableUnPossessionOutlineHandle;
	UPROPERTY()
	FTimerHandle P_ControlLogicHandle;
	UPROPERTY()
	FTimerHandle P_StopDashHandle;
	UPROPERTY()
	FTimerHandle CheckIfFallOutOfWorldHandle;
	UPROPERTY()
	FTimerHandle AI_SetCharacterMovementSpeedHandle;
	UPROPERTY()
	FTimerHandle AI_PlaySpawnSoundHandle;
	UPROPERTY()
	FTimerHandle DestroyCondenseMaterialsProcessHandle;
	UPROPERTY()
	FTimerHandle AI_HideHpBarHandle;
	UPROPERTY()
	FTimerHandle P_DetectIfPlayerPointingAtOutlineObjectsHandle;
	UPROPERTY()
	FTimerHandle P_TriggerCriticalDamageIndicatorHandle;
	UPROPERTY()
	FTimerHandle P_OpenDeathMenuHandle;
	UPROPERTY()
	FTimerHandle P_DeathHandle;
	UPROPERTY()
	FTimerHandle SpawnCondenseMaterialsProcessHandle;

	void AI_SetupData();
	void AI_StopAllAbilities();
	void AI_SetupHpBar();
	void AI_ReceivedDamage(const EDamageReason InDamageReason);
	void AI_DestroyHpBar();
	void AI_ShowDamageNumber(const float InDamageAmount) const;
	void AI_SpawnUsbOnDeath() const;
	void AI_TriggerMutantExplosion();
	void AI_HideHpBar() const;
	void AI_ClearShootPause();
	void AI_SetCharacterMovementSpeed();

	void P_SetupData();
	void P_SetupUI();
	void P_PossessEnemy();
	void P_PossessionLogic(const TObjectPtr<AEnemy>& InEnemy);
	void P_PossessionBarLogic();
	void P_Rotate(const FInputActionValue& Value);
	void P_Reload();
	void P_Dash();
	void P_OnEnemyDeathEvents(const int32 InXp, const float InPossessionTimeValue);
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
	void P_TutorialPressKeyToContinueLogic();
	void P_QuickMeleeAttack();
	void P_ProcessMeleeAttack();
	void P_ApplySecretReward(const int32 InIndex);
	void P_SetupNavArea();
	void P_OnVariationSelection(const ECharacterType InCharacterType);
	void P_ReceiveCurrentlySelectedVariation();
	void P_Jump();
	void P_Stomp();
	void P_SetupLevelUpAppliance(const int32 InLevel);
	void P_StompImpact(const FVector& InHitLocation);
	void P_Death();
	void P_ControlLogic();
	void P_DetectIfPlayerPointingAtOutlineObjects();
	void P_TriggerCriticalDamageIndicator() const;
	void P_OpenDeathMenu() const;
	void P_ContinuousFovChange();
	void P_StopDash() const;
	//UFUNCTION()
	//void P_UpdateHitDamageIndicators() const;

	void SetPressKeyState(const bool bState) { bPressKeyEnabled = bState; }
	void TickAbilities(const float InDeltaTime);
	void SetupStimuliSource();
	void ChangeXRayState(const bool bState) const;
	void SetupUserSettings() const;
	void ClearTimers();
	void ProcessSpawnEffects();
	void AddXp(const int32 InXp) const;
	void AddHp(const int32 InHp);
	void ProcessDamageReason(const EDamageReason InDamageReason) const;
	void ProcessKillDamageReason(const EDamageReason InDamageReason) const;
	virtual void Landed(const FHitResult& Hit) override;
	void CheckIfFallOutOfWorld();
	void SpawnCondenseMaterialsProcess();
	void DestroyCondenseMaterialsProcess();
	void DisablePossessionOutline() const;
	void DisableUnPossessionOutline() const;
};