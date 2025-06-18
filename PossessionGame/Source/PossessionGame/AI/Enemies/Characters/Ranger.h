#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Ranger.generated.h"

class ATriggerSphere;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ARanger : public AEnemy
{
	GENERATED_BODY()
public:
	ARanger();
	
	void AI_ToggleGrenadeLaunchTrigger(const bool bState) const;
	void AI_LaunchGrenade();

	void P_LaunchGrenade();
	
	void SpawnShield();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Launcher", meta = (AllowPrivateAccess = true))
	float GrenadeLaunchTriggerRadiusInMetres = 10.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> GrenadeLauncherMuzzleFlashComponent;
	UPROPERTY()
	TObjectPtr<ATriggerSphere> AIGrenadeLaunchTriggerSphere;
	UPROPERTY()
	FTimerHandle TryTriggeringGrenadeLaunchHandle;

	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;
	UFUNCTION()
	void AI_OnPlayerEnterGrenadeLaunchTrigger(AActor* OverlappedActor, AActor* OtherActor);

	virtual void P_Setup() override;
	virtual void P_SetupColorsForUI() override;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void GrenadeLauncherMuzzleFlashLogic();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitiateDeath() override;
	void SetupGrenadeLaunchTriggerSphere();
	void DestroyGrenadeLaunchTriggerSphere() const;
	void TryTriggeringGrenadeLaunch();
};
