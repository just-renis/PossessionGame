#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "MinionBoss.generated.h"

class UNiagaraComponent;
class ATriggerSphere;

UCLASS()
class POSSESSIONGAME_API AMinionBoss : public AEnemy
{
	GENERATED_BODY()
public:
	void AI_ToggleStompTrigger(const bool bState) const;

	TObjectPtr<UNiagaraComponent> GetLaser() const { return LaserVfx; }
	TObjectPtr<UNiagaraComponent> GetLaserEyeBall() const { return LaserEyeBallVfx; }
	
	void TriggerStompCameraShake() const;
	void EnableLaserEyeBall();
	void EnableLaser();
	void EnableDistanceCheckForLaserEye();
private:
	AMinionBoss();
	
	int32 CurrentWeaponIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Stomp", meta = (AllowPrivateAccess = true))
	float StompTriggerRadiusInMetres = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> StompCameraShake;
	UPROPERTY()
	TArray<TObjectPtr<AWeapon>> Weapons;
	UPROPERTY()
	TObjectPtr<ATriggerSphere> StompTriggerSphere;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LaserEyeBallVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LaserVfx;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_ShootLogic() override;
	virtual void AI_Shoot() override;
	virtual void AI_SetCanShoot(const bool bState) override;
	virtual void AI_CancelShooting() override;
	virtual void AI_InitiateShooting() override;

	virtual TObjectPtr<AWeapon>& GetWeapon() override; // For getting singular weapon data only.
	
	virtual void BeginPlay() override;
	virtual void HandleDeactivation() override;
	virtual void DestroyThis() override;
	void SetupStompTrigger();
	void DestroyStompTriggerSphere() const;
	virtual void InitiateDeath() override;
	UFUNCTION()
	void OnPlayerEnterStompTrigger(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void CheckDistanceOfLaserEyeTrigger();
};
