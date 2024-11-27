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
	TObjectPtr<UEnemyAnimationInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<AEnemyAIController> AI;
	UPROPERTY()
	TObjectPtr<UEnhancedPlayerInput> P_InputManager;
	UPROPERTY()
	FGameplayTagContainer GameplayTags;

	bool GetIsAlive() { return bIsAlive; }
	bool GetIsPossessed() { return bIsPossessed; }
	bool GetIsPossessable();
	bool CanAbilityBeExecuted(int32 AbilityIndex) { return Abilities[AbilityIndex]->CanExecute(); }
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override { return GameplayTags.HasTag(TagToCheck); }
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override { return GameplayTags.HasAll(TagContainer); }
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override { return GameplayTags.HasAny(TagContainer); }
	float GetSkeletalMeshHeight();
	ECharacterType GetCharacterName() { return CharacterName; }
	TObjectPtr<AWeapon>& GetWeapon() { return Weapon; }
	TObjectPtr<UCameraComponent> GetCamera() { return Camera; }
	TObjectPtr<UEnemyAnimationInstance>& GetAnimInstance() { return AnimInstance; }

	void AI_InitiateShooting();
	virtual void AI_ShootLogic();
	virtual void AI_ReloadLogic();
	void AI_UseAbility(int32 AbilityIndex);
	void AI_SetMoveMode(EMoveMode InMoveMode);
	void AI_SetCanShoot(bool bState);

	void P_UseAbility(int32 AbilityIndex);
	void P_UpdateAmmoHUD(int32 WeaponAmmo);
	void P_UpdateAmmoHUD(int32 WeaponAmmo, int32 MaxAmmo);
	void P_UpdateAmmoHUD(int32 FirstWeaponAmmo, int32 SecondWeaponAmmo, int32 MaxAmmo);
	void P_StopTime(); // NOT USED ANYWHERE YET.

	void SetCanReload(bool bState) { bCanReload = bState; }
	void SetCanShootAndReload(bool bState) { bCanShoot = bState; bCanReload = bState; }
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	void ToggleCrouch();
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	void InitiateDeath();
	void ReceiveDamage(float Damage);
protected:
	bool bCanRun = true;
	bool bCanReload = true;
	bool bIsPossessed = false;
	bool bCanShoot = false;
	float MaxHp;
	float CurrentHp;
	ECharacterType CharacterName;
	FVector2D MouseSensitivity = FVector2D(1.f, 1.f);
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

	virtual void AI_SetupAbilities();
	virtual void AI_Setup();
	virtual void AI_SetupWeapon();
	virtual void AI_Shoot();
	void AI_LoadDataIfNotLoaded(ECharacterType PrimaryDataAssetName);
	virtual void AI_SetController();
	virtual void AI_PlayShootMontage();

	virtual void P_SetupAbilities();
	virtual void P_Setup();
	virtual void P_Shoot(bool bFirstWeapon);
	virtual void P_SetupLeftClickInput(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputTriggeredState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupLeftClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent);
	virtual void P_SetupRightClickInput(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent);
	virtual void P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_SetupRightClickInputTriggeredState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_SetupRightClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) {}
	virtual void P_DisableEnemy();
	void P_LoadDataIfNotLoaded(ECharacterType PrimaryDataAssetName);
	void P_TemporallyToggleRunning(bool bState);
	virtual void P_SetupColorsForUI() {}

	void SetCurrentHp(float NewCurrentHp) { CurrentHp = NewCurrentHp; }
	void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	virtual void BeginPlay() override;
	virtual void SetAnimInstance();
	void SetMaxWalkSpeed(float Speed);
	void SetInitialWalkSpeed(bool bState);
	void SetupAbilities(TMap<EAbility, TSoftObjectPtr<UAbilityData>>& AbilityData);
	void AttachDestructible(TObjectPtr<ADestructible> Destructible, FName SocketName);
	virtual void HandleDeactivation();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
private:
	bool bShootPauseActive = false;
	bool bCanPossessOtherEnemy = false;
	bool bIsAlive = true;
	bool bIsUnloadLevel = false;
	bool bIsRunning = false;
	float PossessionBarTimer = 0.f;
	EMoveMode CurrentMoveMode = EMoveMode::Walk;
	UPROPERTY(EditAnywhere, Category = "Nav", meta = (AllowPrivateAccess = true))
	TSubclassOf<ANavAreaModifier> P_NavAreaDefaultClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> AI_HpBarComponent;
	UPROPERTY()
	TObjectPtr<UAIHpBarUI> AI_HpBar;
	UPROPERTY()
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;
	UPROPERTY()
	TObjectPtr<ANavAreaModifier> NavAreaModifier;
	EAbility GetAbilityFromCharacterName(int32 AbilityIndex);

	void AI_SetupData();
	void AI_Reload();
	void AI_StopAllAbilities();
	void AI_SetShootPause();
	UFUNCTION()
	void AI_ClearShootPause();
	UFUNCTION()
	void AI_SetCharacterMovementSpeed();

	void P_SetupData();
	void P_SetupUI();
	void P_PossessEnemy();
	void P_PossessionLogic(TObjectPtr<AActor> Actor);
	void P_PossessionBarLogic();
	void P_Move(const FInputActionValue& Value);
	void P_Rotate(const FInputActionValue& Value);
	void P_Reload();
	void P_ToggleRunning(bool bState);
	void P_OnEnemyDeathEvents();
	void P_ToggleGodMode();
	void P_UpdateAbilitiesUI();
	void P_UpdateAbilityCooldownUITextColor(int32 Index);
	void P_StopAllAbilities();
	void P_HandleEscapeClick();
	void P_TriggerGlitchingEffect();
	UFUNCTION()
	void P_TurnOffGlitchingEffect() const;
	UFUNCTION()
	void P_Death();
	UFUNCTION()
	void P_ControlLogic();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DestroyAI();
	void TickAbilities(float DeltaTime);
	void RestartLevelAfterDelay();
	void ClearTimers();
	void SetupStimuliSource();
	void SetMesh(ECharacterType PrimaryDataAssetName);
	void P_SetupNavArea();
	void ChangeXRayState(bool bState);
	UFUNCTION()
	void CheckIfFallOutOfWorld();
	UFUNCTION()
	void DestroyThis();
};