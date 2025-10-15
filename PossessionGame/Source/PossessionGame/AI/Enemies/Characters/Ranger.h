#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Ranger.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ARanger : public AEnemy
{
	GENERATED_BODY()
public:
	ARanger();
	
	void AI_ToggleGrenadeTrigger(const bool bState) const;
	void AI_LaunchGrenade();

	void P_LaunchGrenade();
	void P_InitiateThrowMine();
	
	void SpawnShield();
private:
	bool bIsMineSpecialUpgrade1Taken = false;
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Launcher", meta = (AllowPrivateAccess = true))
	float GrenadeTriggerRadiusInMetres = 10.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> AI_GrenadeMuzzleFlashComponent;
	UPROPERTY()
	TObjectPtr<USphereComponent> AI_GrenadeTrigger;
	UPROPERTY()
	FTimerHandle TryTriggeringGrenadeHandle;
	UPROPERTY()
	FTimerHandle MineThrowHandle;

	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;
	void AI_DestroyGrenadeTrigger() const;
	void AI_TryTriggeringGrenade();
	virtual void AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage) override;
	UFUNCTION()
	void AI_OnPlayerEnterGrenadeTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void P_Setup() override;
	virtual void P_SetupColorsForUI() override;
	void P_ChangeRangerVariationMuzzle();
	void P_ThrowMine();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void GrenadeMuzzleFlashLogic();
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitiateDeath() override;
	virtual void SetEmpEffect(const float InEffectDuration) override;
	virtual void EmpEffectEnded() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
