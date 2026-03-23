#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Interfaces/DamageReasonProvider.h"
#include "Tank.generated.h"

enum class EUpgrade : uint8;
enum class EUpgradeAttribute : uint8;
class APoolObject;

UCLASS()
class POSSESSIONGAME_API ATank : public AEnemy, public IDamageReasonProvider
{
	GENERATED_BODY()
public:
	void AI_SpawnShield();
	
	virtual void P_Shoot() override;
	void P_InitiateThrowGravitationalPull();
	void P_ToggleChargeMinigun(const bool bState);
	
	virtual EDamageReason GetDamageReason() const override { return EDamageReason::TankSlam; }
	
	void SetInputState(const bool bIsLocked) override;
private:
	ATank();

	bool bIsSpecialDamageReductionUpgradeTaken = false;
	bool bIsSpecialIgnoreNegativeEffectsUpgradeTaken = false;
	int32 DecalLoopsCount = 0;
	int32 AI_ShieldScalingLoopsCount = 0;
	float DecalDissolveValue = 0.f;
	float AI_ShieldScale = 3.f;
	FTimerHandle AI_ShieldScalingHandle;
	FTimerHandle SlamDecalHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Slam", meta = (AllowPrivateAccess = true))
	float DecalLifespan = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slam", meta = (AllowPrivateAccess = true))
	float LegendaryStunDuration = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slam", meta = (AllowPrivateAccess = true))
	float LegendaryStunRadiusMultiplier = 1.5f;
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
	void AI_ProcessShieldScaling();
	void AI_ResetShield();
	void AI_AddLegendaryUpgrade(const EUpgrade InUpgradeName, const EUpgradeAttribute InUpgradeAttribute);
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;
	virtual void AI_ReceiveEmpEffect(const float InEffectDuration) override;
	virtual void AI_PlayDeathElectrification() override;

	bool P_GetIsLeftMouseInputNotPressed() const;
	virtual float P_GetStompDamage() override;
	virtual float P_GetStompSize() override;
	
	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputCompletedState() override;
	virtual void P_SetupShiftInput() override {}
	virtual void P_BindQuickMeleeAction() override {}
	virtual void P_BindSlideAction() override {}
	virtual void P_Destroy() override;
	void P_StopChargingMinigun();
	virtual void P_WeaponShoot() override;
	void P_ProcessSlamDecal();
	virtual void P_MakeCameraShake(const ECameraShake InCameraShakeName) override;
	void P_LegendarySlamStun(const FVector& InSlamLocation);
	UFUNCTION()
	void P_StopMinigun();
	
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void InitiateDeath() override;
	virtual void SetActionSpeed(const float InSpeed) override;
};