#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "MinionBoss.generated.h"

class USphereComponent;
class UNiagaraComponent;

DECLARE_DELEGATE(FStopLaser);

UCLASS()
class POSSESSIONGAME_API AMinionBoss : public AEnemy
{
	GENERATED_BODY()
public:
	float GetStompRadius() const { return StompRadiusInMeters * 100.f; }
	float GetStompDamage() const { return StompDamage; }
	FStopLaser& GetStopLaserDelegate() { return StopLaser; }
	TObjectPtr<UCapsuleComponent> GetLaserCollision() const { return LaserCollision; }
	TObjectPtr<UNiagaraComponent> GetLaser() const { return LaserVfx; }
	TObjectPtr<UNiagaraComponent> GetLaserEyeBall() const { return LaserEyeBallVfx; }

	void SetIsStompReady(const bool bState) { bStompIsReady = bState; }
	void ActivateStompTrigger();
	void TriggerStompCameraShake() const;
	void EnableLaserEyeBall();
	void EnableLaser();
	void EnableDistanceCheckForLaserEye();
	void OnLaserFinish();
	void OnStompFinish();
	void ResetMoveDirectlyToPlayer();
private:
	AMinionBoss();

	FStopLaser StopLaser;
	
	bool bStompIsReady = false;
	bool bIsMoveDirectlyToPlayerTriggered = false;
	int32 CurrentWeaponIndex = 0;
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
	TObjectPtr<USphereComponent> StompTrigger;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LaserEyeBallVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LaserVfx;
	UPROPERTY()
	TObjectPtr<AEnemy> Player;
	UPROPERTY()
	FTimerHandle CheckLaserEyeDistanceHandle;
	UPROPERTY()
	FTimerHandle DealLaserDamageToPlayerHandle;
	UPROPERTY()
	FTimerHandle MoveDirectlyToPlayerHandle;

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
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void HandleDeactivation() override;
	virtual void DestroyThis() override;
	void DestroyStompTriggerSphere() const;
	virtual void InitiateDeath() override;
	void ToggleStompTrigger(const bool bState) const;
	virtual void SetEmpEffect(const float InEffectDuration) override;
	virtual void EmpEffectEnded() override;
	virtual void ReceiveDamage(const float InDamage, const EDamageReason InDamageReason) override;
	void CheckDistanceOfLaserEyeTrigger();
	void DealLaserDamageToPlayer() const;
	virtual void SetUnHijackOutline(const bool bState) const override {}
	void SetMoveDirectlyToPlayer();
	UFUNCTION()
	void OnPlayerEnterStompTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnLaserCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnLaserCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
