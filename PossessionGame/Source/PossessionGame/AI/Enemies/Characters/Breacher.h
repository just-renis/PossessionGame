#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Breacher.generated.h"

UCLASS()
class POSSESSIONGAME_API ABreacher : public AEnemy
{
	GENERATED_BODY()
public:
	void AI_EnableTriggeringBash();
	
	void P_ThrowBarrel();
	void P_Bash(const float InImpulseStrength);
	void P_ThrowDynamite();
	virtual void P_RemoveReloadSound() override;
private:
	ABreacher();

	bool bImmuneToExplosions = false;
	bool bAI_ShootSpeedDoubled = false;
	bool bIsBashing = false;
	bool bIsBashInitiated = false;
	bool bIsBashTriggerInitiated = false;
	FTimerHandle AI_TriggerBashHandle;
	FTimerHandle AI_TriggerBashImpulse;
	FTimerHandle StopBashHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float AI_BashImpulse = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float AI_DefaultBashDamage = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float AI_BashStunDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float AI_BashStartInAnimation = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float AI_BashDuration = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash Overlap Sweep", meta = (AllowPrivateAccess = true))
	float AI_BashCapsuleHalfHeight = 107.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash Overlap Sweep", meta = (AllowPrivateAccess = true))
	float AI_BashCapsuleRadius = 64.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash Overlap Sweep", meta = (AllowPrivateAccess = true))
	float AI_BashCapsuleForwardUnits = 35.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float P_BashStunDuration = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bash", meta = (AllowPrivateAccess = true))
	float P_DefaultBashDamage = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> BashCameraShake;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> BashWindUpSfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> AI_BashWindUpFistVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> AI_BashDashVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> AI_BashDashTrailVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> P_BashVfx;

	virtual float AI_GetShootPause() override;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	void AI_TryTriggeringBash();
	void AI_Bash();
	void AI_ActivateBashWindUpVfx();
	void AI_ActivateBashDashVfx();
	void AI_ActivateBashDashTrailVfx();
	virtual void AI_DestroyVfxs() override;
	virtual void AI_DisableActions(const float InEffectDuration, const EDisablingEffect InEffectName) override;
	UFUNCTION()
	void AI_BashHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void P_Setup() override;
	virtual void P_SetupRightClickInput(const TObjectPtr<UEnhancedInputComponent> InEnhancedInputComponent) override;
	void P_StopAIBashLogic();
	virtual void P_WeaponShoot() override;
	UFUNCTION()
	void P_BashHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void BeginPlay() override;
	void DetonateDynamites();
	virtual void ClearTimers() override;
	void StopBash();
	void TriggerBashOverlap();
	virtual void AllDisablingActionsFinished() override;
	void DealDamageToDestructible(const TObjectPtr<AActor>& InDestructible);
};
