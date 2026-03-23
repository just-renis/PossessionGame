#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Spy.generated.h"

UCLASS()
class POSSESSIONGAME_API ASpy : public AEnemy
{
	GENERATED_BODY()
public:
	bool GetXrayState() const { return bXrayEnabled; }
	
	void SetXrayState(const bool bState);
	void StartTeleportProcess();
	void FinishTeleportProcess();
	void SetFirstAbilityActiveState(const bool bState);
private:
	ASpy();
	
	bool bXrayEnabled = false;
	bool bIsFirstAbilityActive = false;
	bool bIsSecondAbilityActive = false;
	float XrayColorRadius = 3.f;
	float XrayRadialRadius = 1.f;
	float XrayDarkness = 1.f;
	float InvincibilityColorRadius = 3.f;
	float InvincibilityRadialRadius = 1.f;
	float FirstAbilityAbsorbedDamage = 0.f;
	int32 XrayCountLoops = 0;
	int32 InvincibilityCountLoops = 0;
	int32 FirstAbilityAbsorbedAmountRequired = 75;
	FTimerHandle ProcessXrayMaterialsHandle;
	FTimerHandle ProcessInvincibilityMaterialHandle;
	UPROPERTY(EditDefaultsOnly, Category = "AI Variables", meta = (AllowPrivateAccess = true))
	float AI_DashSpeedMultiplier = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Teleport;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Xray;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Invincibility;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_DarkXray;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> TeleportKillCameraShake;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TeleportLinesVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> InvincibilityVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> AI_TeleportVfx;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Teleport;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Xray;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Invincibility;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_DarkXray;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> AI_DashSfx;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_SetController() override;
	virtual void AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage) override;
	void AI_DisableTeleportVfx() const;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_OnReachedNonEqsPoint() override;
	void AI_UseDashToLocationAbility();
	virtual void AI_PlayDeathElectrification() override;

	bool P_IsSnatchAbilityAvailable() const;
	virtual float P_GetMeleeDamage() override;
	virtual float P_GetStompDamage() override;

	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputTriggeredState() override {}
	virtual void P_WeaponShoot() override;
	virtual void P_MakeCameraShake(const ECameraShake InCameraShakeName) override;
	virtual void P_ProcessMeleeAttack() override;
	virtual void P_InitiateStomp() override;
	virtual void P_RemoveReloadSound() override;
	virtual void P_FinishHijackProcess() override;
	virtual void P_ReceiveStunEffect(const float InEffectDuration, const float InDamage, FVector InHitDirection) override;

	virtual float ReceiveMovementSpeedModification() const override;
	
	virtual void BeginPlay() override;
	void SetTeleportMaterialValue(const float InValue);
	void ProcessXrayMaterialsActivation();
	void ProcessXrayMaterialsDeactivation();
	void ProcessInvincibilityMaterialActivation();
	void ProcessInvincibilityMaterialDeactivation();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};