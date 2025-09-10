#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Trickster.generated.h"

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
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Launcher", meta = (AllowPrivateAccess = true))
	float PoisonGrenadeTriggerRadiusInMetres = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "Clones", meta = (AllowPrivateAccess = true))
	TArray<FVector> CloneSpawnOffsets;
	UPROPERTY()
	TObjectPtr<USphereComponent> AI_PoisonGrenadeTrigger;
	UPROPERTY()
	FTimerHandle TryTriggeringPoisonGrenadeHandle;
	
	ATrickster();
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;
	void AI_TryTriggeringPoisonGrenadeLaunch();
	void AI_DestroyPoisonGrenadeTrigger();
	UFUNCTION()
	void AI_OnPlayerEnterPoisonGrenadeTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void P_Setup() override;
	virtual void P_SetupColorsForUI() override;

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitiateDeath() override;
	virtual void SetEmpEffect(const float InEffectDuration) override;
	virtual void EmpEffectEnded() override;
};