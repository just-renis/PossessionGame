#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Tank.generated.h"

class APoolObject;

UCLASS()
class POSSESSIONGAME_API ATank : public AEnemy
{
	GENERATED_BODY()
public:
	void AI_SpawnShield();
	
	virtual void P_Shoot() override;
	void P_InitiateThrowGravitationalPull();
private:
	ATank();

	int32 DecalLoopsCount = 0;
	float DecalDissolveValue = 0.f;
	FTimerHandle SlamDecalHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Slam", meta = (AllowPrivateAccess = true))
	float DecalLifespan = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slam", meta = (AllowPrivateAccess = true))
	FVector DecalSize;
	UPROPERTY(EditDefaultsOnly, Category = "Slam", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInstance> MI_Slam;
	UPROPERTY(EditDefaultsOnly, Category = "Slam", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> SlamCameraShake;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SlamVfx;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Slam;
	UPROPERTY()
	TObjectPtr<APoolObject> AI_Shield;
	
	virtual void AI_ReloadLogic() override;
	virtual void AI_SetCanShoot(const bool bState) override;
	virtual void AI_PlayShootMontage() override;
	virtual void AI_SetupWeapon() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage) override;

	bool P_GetIsLeftOrRightInputNotPressed();
	virtual float P_GetStompDamage() override;
	virtual float P_GetStompSize() override;
	
	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputCompletedState() override;
	virtual void P_SetupShiftInput() override {}
	virtual void P_BindQuickMeleeAction() override {}
	virtual void P_BindSlideAction() override {}
	virtual void P_Destroy() override;
	void P_ToggleChargeMinigun(const bool bState);
	void P_StopChargingMinigun();
	virtual void P_WeaponShoot() override;
	void P_ProcessSlamDecal();
	virtual void P_MakeCameraShake(const ECameraShake InCameraShakeName) override;
	UFUNCTION()
	void P_StopMinigun();
	
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void InitiateDeath() override;
};