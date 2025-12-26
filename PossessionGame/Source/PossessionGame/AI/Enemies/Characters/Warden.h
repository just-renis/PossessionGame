#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Warden.generated.h"

UCLASS()
class POSSESSIONGAME_API AWarden : public AEnemy
{
	GENERATED_BODY()
public:
	void TriggerBeam();
	void TriggerBeamDamage();
private:
	AWarden();

	FTimerHandle BeamTriggerHandle;
	FTimerHandle BeamDisableCollisionHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	float DamageTimeSpan = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	float Damage = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	float DistanceMulti = 0.00111f;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> BeamWeapon;
	UPROPERTY()
	TObjectPtr<UCapsuleComponent> BeamCollision;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BeamVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> BeamFireSfx;

	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;
	virtual void AI_DisableActions(const float InEffectDuration, const EDisablingEffect InEffectName) override;

	virtual void BeginPlay() override;
	void TryTriggeringBeamShot();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DisableBeamCollision();
	virtual void InitiateDeath() override;
	virtual void ReceiveDamage(const float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent) override;
	virtual void ClearTimers() override;
	virtual void ApplyEmpEffect(const float InEffectDuration) override;
	UFUNCTION()
	void OnBeamCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
