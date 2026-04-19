#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "SoldierBoss.generated.h"

class UBackpackAnimInstance;

UCLASS()
class POSSESSIONGAME_API ASoldierBoss : public AEnemy
{
	GENERATED_BODY()
public:
	void MarkMissilesLaunchAbilityReady();
	void StartMissilesLaunchPhase(const float InDuration);
	void FinishedMissilesLaunch() const;
private:
	ASoldierBoss();

	FTimerHandle ActivateMissilesLaunchHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Backpack", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimationAsset> BackpackOpenAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Backpack", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimationAsset> BackpackCloseAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Backpack", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Backpack;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> TakeDamageSfx;
	UPROPERTY()
	TObjectPtr<UBackpackAnimInstance> BackpackAnimInstance;

	virtual void AI_SetupWeapon() override;
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;

	virtual void BeginPlay() override;
	virtual void ApplyEmpEffect(const float InEffectDuration) override;
	virtual void ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent) override;
	virtual void InitiateDeath() override;
	void ActivateMissilesLaunch();
	virtual void ClearTimers() override;
};
