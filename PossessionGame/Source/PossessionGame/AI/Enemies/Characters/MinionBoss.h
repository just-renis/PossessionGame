#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "MinionBoss.generated.h"

class USphereComponent;
class UNiagaraComponent;

DECLARE_DELEGATE(FStopLaser);
DECLARE_DELEGATE_RetVal(bool, FRequestLaserActivityState);

UCLASS()
class POSSESSIONGAME_API AMinionBoss : public AEnemy
{
	GENERATED_BODY()
public:
	float GetStompRadius() const { return StompRadiusInMeters * 100.f; }
	float GetStompDamage() const { return StompDamage; }
	FStopLaser& GetStopLaserDelegate() { return StopLaser; }
	FRequestLaserActivityState& GetLaserActivityStateDelegate() { return RequestLaserActivityState; }
	TObjectPtr<UCapsuleComponent> GetLaserCollision() const { return LaserCollision; }
	TObjectPtr<UNiagaraComponent> GetLaser() const { return LaserVfx; }
	TObjectPtr<UNiagaraComponent> GetLaserEyeBall() const { return LaserEyeBallVfx; }

	void TriggerStompCameraShake() const;
	void TriggerLaserCameraShake() const;
	void EnableLaserEyeBall();
	void EnableLaser();
	void EnableDistanceCheckForLaserEye();
	void OnLaserFinish();
	void OnStompFinish();
	void ResetMoveDirectlyToPlayer();
	void SetStompAvailability(const bool bState) { bIsStompAvailable = bState; }
private:
	AMinionBoss();

	FStopLaser StopLaser;
	FRequestLaserActivityState RequestLaserActivityState;
	
	bool bIsMoveDirectlyToPlayerTriggered = false;
	bool bIsStompAvailable = false;
	float WeaponShootCooldownTimer;
	int32 CurrentWeaponIndex = 0;
	FTimerHandle CheckLaserEyeDistanceHandle;
	FTimerHandle DealLaserDamageToPlayerHandle;
	FTimerHandle MoveDirectlyToPlayerHandle;
	FTimerHandle TryTriggerStompHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Shoot Cooldown", meta = (AllowPrivateAccess = true))
	float WeaponShootCooldown;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Stomp", meta = (AllowPrivateAccess = true))
	float StompTriggerRadiusInMeters = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float MoveDirectlyToPlayerInterval = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Laser Collision", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> LaserCollision;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Defeated;
	UPROPERTY()
	TArray<TObjectPtr<AWeapon>> Weapons;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LaserEyeBallVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LaserVfx;
	UPROPERTY()
	TObjectPtr<AEnemy> Player;

	virtual TObjectPtr<AWeapon>& GetWeapon() override; // For getting singular weapon data only.
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_ShootLogic() override;
	virtual void AI_Shoot() override;
	virtual void AI_SetCanShoot(const bool bState) override;
	virtual void AI_CancelShooting() override;
	virtual void AI_InitiateShooting() override;
	virtual void AI_PlayDeathExplosion() override;
	virtual void AI_PlayDeathElectrification() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;
	virtual void AI_DisableActions(const float InEffectDuration, const EDisablingEffect InEffectName) override;
	
	virtual void BeginPlay() override;
	virtual void HandleDeactivation(const FVector& InImpulse) override;
	virtual void DestroyThis() override;
	virtual void InitiateDeath(const FVector& InImpulse) override;
	void TryTriggerStomp();
	virtual void ReceiveDamage(const float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent) override;
	void CheckDistanceOfLaserEyeTrigger();
	void DealLaserDamageToPlayer() const;
	virtual void AI_SetUnHijackOutline(const bool bState) override {}
	void SetMoveDirectlyToPlayer();
	virtual void ClearTimers() override;
	virtual void ApplyEmpEffect(const float InEffectDuration) override;
	UFUNCTION()
	void OnLaserCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnLaserCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
