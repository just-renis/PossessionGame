#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputTriggers.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagAssetInterface.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "Enemy.generated.h"

class APossessionGameGameModeBase;
enum class ESound : uint8;
class UAIHpBarUI;
class UWidgetComponent;
class UPrimaryDataAssetsManager;
enum class EVfx : uint8;
class ADestructible;
enum class EMoveMode : uint8;
class ANavAreaModifier;
class USphereComponent;
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
class POSSESSIONGAME_API AEnemy : public ACharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
public:
	AEnemy();
	
	UPROPERTY()
	TObjectPtr<UEnhancedPlayerInput> P_InputManager;
	UPROPERTY()
	FGameplayTagContainer GameplayTags;

	bool GetIsAlive() const { return bIsAlive; }
	bool GetIsPossessed() const { return bIsPossessed; }
	bool GetIsPossessable() const;
	bool GetIsActionInMotion() const { return bIsActionInMotion; }
	bool CanAbilityBeExecuted(const int32 AbilityIndex) { return Abilities[AbilityIndex]->CanExecute(); }
	virtual bool HasMatchingGameplayTag(const FGameplayTag TagToCheck) const override { return GameplayTags.HasTag(TagToCheck); }
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override { return GameplayTags.HasAll(TagContainer); }
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override { return GameplayTags.HasAny(TagContainer); }
	float GetSkeletalMeshHeight() const;
	ECharacterType GetCharacterName() const { return CharacterName; }
	TObjectPtr<APossessionGameGameModeBase> GetGameModeBase() const { return GameModeBase; }
	TObjectPtr<AEnemyAIController> GetAI() const { return AI; }
	TObjectPtr<AWeapon>& GetWeapon() { return Weapon; }
	TObjectPtr<UCameraComponent> GetCamera() { return Camera; }
	TObjectPtr<UEnemyAnimationInstance>& GetAnimInstance() { return AnimInstance; }

	void AI_InitiateShooting();
	virtual void AI_ShootLogic();
	virtual void AI_ReloadLogic();
	void AI_UseAbility(const int32 AbilityIndex);
	void AI_SetMoveMode(const EMoveMode InMoveMode);
	void AI_SetShootPause();
	virtual void AI_SetCanShoot(const bool bState);

	void P_UseAbility(const int32 AbilityIndex);
	void P_UpdateAmmoHUD(const int32 WeaponAmmo) const;
	void P_UpdateAmmoHUD(const int32 WeaponAmmo, const int32 MaxAmmo) const;
	void P_UpdateAmmoHUD(const int32 FirstWeaponAmmo, const int32 SecondWeaponAmmo, const int32 MaxAmmo) const;
	void P_StopTime(); // NOT USED ANYWHERE YET.

	void SetActionInMotion(const bool bState);
	virtual void Tick(const float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	void ToggleCrouch();
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	virtual void InitiateDeath();
	void ReceiveDamage(const float InDamage);
	void PlaySound(const ESound InSoundName);
	void SetEMPEffect();
	void SetPossessionOutline(const bool bState) const;
	void SetPossessionOutlineTimer();
	UFUNCTION()
	void DisablePossessionOutline() const;
protected:
	bool bCanRun = true;
	bool bCanReload = true;
	bool bIsPossessed = false;
	bool bCanShoot = false;
	bool bIsActionInMotion = false;
	bool bShootPauseActive = false;
	float MaxHp;
	float CurrentHp;
	ECharacterType CharacterName;
	FVector2D MouseSensitivity = FVector2D(1.f, 1.f);
	UPROPERTY()
	TObjectPtr<UEnemyAnimationInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<AEnemyAIController> AI;
	UPROPERTY()
	TObjectPtr<UPlayerHUD> P_HUD;
	UPROPERTY()
	TObjectPtr<AWeapon> Weapon;
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY()
	TArray<TObjectPtr<UAbility>> Abilities;
	UPROPERTY()
	TObjectPtr<UAICharacterData> AI_Data;
	UPROPERTY()
	TObjectPtr<UPlayerCharacterData> P_Data;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
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
	virtual void AI_PlayDeathSound() {}
	UFUNCTION()
	virtual void AI_TriggerDeathVoiceLine() {}

	virtual void P_SetupAbilities();
	virtual void P_Setup();
	virtual void P_Shoot(const bool bFirstWeapon);
	virtual void P_SetupLeftClickInput(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupRightClickInput(const TObjectPtr<UEnhancedInputComponent> RightClickInputComponent);
	virtual void P_SetupRightClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_SetupRightClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_SetupRightClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_DisableEnemy();
	void P_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName);
	void P_TemporallyToggleRunning(const bool bState);
	virtual void P_SetupColorsForUI() {}

	void SetCurrentHp(const float NewCurrentHp) { CurrentHp = NewCurrentHp; }
	void SetMaxHp(const float NewMaxHp) { MaxHp = NewMaxHp; }
	virtual void BeginPlay() override;
	virtual void SetAnimInstance();
	void SetMaxWalkSpeed(const float Speed) const;
	void SetInitialWalkSpeed(const bool bState) const;
	void SetupAbilities(const TMap<EAbility, TSoftObjectPtr<UAbilityData>>& AbilityData);
	void AttachDestructible(const TObjectPtr<ADestructible>& Destructible, const FName SocketName) const;
	virtual void HandleDeactivation();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void ClearTimers();
private:
	bool bCanPossessOtherEnemy = false;
	bool bIsAlive = true;
	bool bEnableUnloadLevel = false;
	bool bIsRunning = false;
	bool bIsCriticallyDamaged = false;
	bool bIsAIBarCreated = false;
	bool bPlayerDied = false;
	float PossessionBarTimer = 0.f;
	EMoveMode CurrentMoveMode = EMoveMode::Walk;
	UPROPERTY(EditAnywhere, Category = "Nav", meta = (AllowPrivateAccess = true))
	TSubclassOf<ANavAreaModifier> P_NavAreaDefaultClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> AI_HpBarComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> RunningCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> ReceivingDamageCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> PossessionOutlineMaterial;
	UPROPERTY()
	TObjectPtr<UAIHpBarUI> AI_HpBar;
	UPROPERTY()
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;
	UPROPERTY()
	TObjectPtr<ANavAreaModifier> NavAreaModifier;
	UPROPERTY()
	TObjectPtr<APossessionGameGameModeBase> GameModeBase;
	EAbility GetAbilityFromCharacterName(const int32 AbilityIndex) const;

	void AI_SetupData();
	void AI_StopAllAbilities();
	void AI_SetupHpBar();
	void AI_ReceivedDamage();
	void AI_DestroyHpBar();
	void AI_ShowDamageNumber(const float InDamageAmount) const;
	UFUNCTION()
	void AI_HideHpBar() const;
	UFUNCTION()
	void AI_ClearShootPause();
	UFUNCTION()
	void AI_SetCharacterMovementSpeed();

	void P_SetupData();
	void P_SetupUI();
	void P_PossessEnemy();
	void P_PossessionLogic(const TObjectPtr<AEnemy>& InEnemy);
	void P_PossessionBarLogic();
	void P_Move(const FInputActionValue& Value);
	void P_Rotate(const FInputActionValue& Value);
	void P_Reload();
	void P_ToggleRunning(bool bState);
	void P_OnEnemyDeathEvents();
	void P_ToggleGodMode();
	void P_UpdateAbilitiesUI();
	void P_UpdateAbilityCooldownUI(const int32 Index);
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
	UFUNCTION()
	void P_Death();
	UFUNCTION()
	void P_ControlLogic();
	UFUNCTION()
	void P_DetectIfPlayerPointingAtEnemy();
	UFUNCTION()
	void P_TriggerCriticalDamageIndicator() const;
	UFUNCTION()
	void P_TriggerRunningCameraShake() const;
	UFUNCTION()
	void P_OpenDeathMenu() const;
	UFUNCTION()
	void P_UpdateHitDamageIndicators() const;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DestroyAI();
	void TickAbilities(const float DeltaTime);
	void SetupStimuliSource();
	void SetMesh(const ECharacterType PrimaryDataAssetName) const;
	void P_SetupNavArea();
	void ChangeXRayState(const bool bState) const;
	void SetupUserSettings() const;
	UFUNCTION()
	void CheckIfFallOutOfWorld();
	UFUNCTION()
	void DestroyThis();
};