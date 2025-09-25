#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Warden.generated.h"

UCLASS()
class POSSESSIONGAME_API AWarden : public AEnemy
{
	GENERATED_BODY()
public:
	TSubclassOf<UCameraShakeBase> GetBeamShotCameraShake() { return BeamShotCameraShake; }
	
	void TriggerBeam();
	void TriggerBeamDamage();
private:
	AWarden();

	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	float DamageTimeSpan = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	float Damage = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	float DistanceMulti = 0.00111f;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> BeamShotCameraShake;
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
	UPROPERTY()
	FTimerHandle BeamTriggerHandle;
	UPROPERTY()
	FTimerHandle BeamDisableCollisionHandle;

	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;

	virtual void BeginPlay() override;
	virtual void SetEmpEffect(const float InEffectDuration) override;
	void TryTriggeringBeamShot();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DisableBeamCollision();
	UFUNCTION()
	void OnBeamCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
