#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Trickster.generated.h"

enum class EMoveMode : uint8;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API ATrickster : public AEnemy
{
	GENERATED_BODY()
public:
	void AI_TogglePoisonGrenadeTrigger(const bool bState) const;
	void AI_LaunchPoisonGrenade();
	
	void P_ThrowPoisonGrenade() const;
	void P_ThrowEmpGrenade() const;

	TArray<FVector> GetAvailableCloneSpawnOffsets(int32 InClonesAmount);
private:
	bool bIsCloneEnemyRunUpgradeTaken = false;
	FTimerHandle TryTriggeringPoisonGrenadeHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Launcher", meta = (AllowPrivateAccess = true))
	float PoisonGrenadeTriggerRadiusInMetres = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "Clones", meta = (AllowPrivateAccess = true))
	float AI_MovementSpeedMultiplierForCloneSpecialUpgrade = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Clones", meta = (AllowPrivateAccess = true))
	TArray<FVector> CloneSpawnOffsets;
	UPROPERTY()
	TObjectPtr<USphereComponent> AI_PoisonGrenadeTrigger;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> AI_CloneAbilityRunVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> CloneUpgradeDashSound;
	
	ATrickster();
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;
	void AI_TryTriggeringPoisonGrenadeLaunch();
	void AI_DestroyPoisonGrenadeTrigger() const;
	virtual void AI_DestroyVfxs() override;
	UFUNCTION()
	void AI_OnPlayerEnterPoisonGrenadeTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void P_Setup() override;
	virtual void P_WeaponShoot() override;

	virtual float ReceiveMovementSpeedModification() const override;

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitiateDeath() override;
	virtual void ReceiveEmpEffect(const float InEffectDuration) override;
	virtual void SetMoveMode(const EMoveMode InMoveMode) override;
	virtual void AllDisablingActionsFinished() override;
};