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
	
	void TriggerBeam() const;
private:
	AWarden();

	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	float DistanceMulti = 0.00111f;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> BeamShotCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Beam Weapon", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> BeamWeapon;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BeamVfx;
	UPROPERTY()
	FTimerHandle BeamTriggerHandle;

	virtual void AI_SetupWeapon() override;

	virtual void BeginPlay() override;
	virtual void SetEmpEffect(const float InEffectDuration) override;
	void TryTriggeringBeamShot();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
