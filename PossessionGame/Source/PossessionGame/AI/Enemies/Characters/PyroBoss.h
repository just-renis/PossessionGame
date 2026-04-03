#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PyroBoss.generated.h"

UCLASS()
class POSSESSIONGAME_API APyroBoss : public AEnemy
{
	GENERATED_BODY()
public:
	float GetStompRadius() const { return StompRadiusInMeters; }
	float GetStompDamage() const { return StompDamage; }
	float GetStunDuration() const { return StunDuration; }
	
	void MarkFloorLavaAbilityReady();
	void MarkStompAbilityReady();
	void MarkBallPhaseAbilityReady();
	void InitiateBallPhase();
	void FinishedBallPhase();
private:
	APyroBoss();

	bool bIsBallPhaseActive = false;
	FTimerHandle ActivateFloorLavaHandle;
	FTimerHandle ActivateStompHandle;
	FTimerHandle ActivateBallPhaseHandle;
	FTimerHandle BallPhaseAttackHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Lava Floor", meta = (AllowPrivateAccess = true))
	float SecondPhaseHpPercentage = 0.7f;
	UPROPERTY(EditDefaultsOnly, Category = "Lava Floor", meta = (AllowPrivateAccess = true))
	float ThirdPhaseHpPercentage = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Stomp", meta = (AllowPrivateAccess = true))
	float StompTriggerRadiusInMeters = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stomp", meta = (AllowPrivateAccess = true))
	float StunDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Ball Phase", meta = (AllowPrivateAccess = true))
	FProjectileData ProjectileData;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SendSignalVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> TakeDamageSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ActivateFlameFloorSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> InitiateFlameFloorSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> StompSfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BallPhaseVfx;
	
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;

	virtual void BeginPlay() override;
	virtual void InitiateDeath() override;
	virtual void ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent) override;
	virtual void ApplyEmpEffect(const float InEffectDuration) override;
	void ActivateFloorLava();
	void ActivateStomp();
	void ActivateBallPhase();
	void ActivateBallPhaseAttack();
	void TriggerBallPhaseAttack();
	virtual void ClearTimers() override;
};
