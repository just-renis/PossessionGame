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
	void AI_TogglePoisonGrenadeLaunchTrigger(const bool bState) const;
	void AI_LaunchPoisonGrenade();
	
	void P_ThrowPoisonGrenade() const;
	void P_ThrowEmpGrenade() const;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Launcher", meta = (AllowPrivateAccess = true))
	float PoisonGrenadeLaunchTriggerRadiusInMetres = 8.f;
	UPROPERTY()
	TObjectPtr<USphereComponent> AI_PoisonGrenadeLaunchTrigger;
	UPROPERTY()
	FTimerHandle TryTriggeringPoisonGrenadeLaunchHandle;
	
	ATrickster();
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;
	void AI_TryTriggeringPoisonGrenadeLaunch();
	UFUNCTION()
	void AI_OnPlayerEnterPoisonGrenadeLaunchTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void P_Setup() override;
	virtual void P_SetupColorsForUI() override;

	virtual void BeginPlay() override;
};