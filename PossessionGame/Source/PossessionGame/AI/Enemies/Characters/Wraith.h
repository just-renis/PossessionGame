#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "Wraith.generated.h"

class UDamageOverTimeData;

UCLASS()
class POSSESSIONGAME_API AWraith : public AEnemy
{
	GENERATED_BODY()
public:
	float AI_GetMeteorShowerLifeSpan() const { return AI_MeteorShowerData->GetAreaLifeSpan(); }
	
	void AI_EnableTriggeringMeteorShower();
	void AI_InitiateMeteorShower();
	void AI_FinishMeteorShower();
	
	void P_InitiateThrowMolotov();
	void P_InitiateThrowTrap();
	void P_InitiateThrowMeteorShower();
private:
	AWraith();

	bool bIsMeteorShowerTriggerInitiated = false;
	FTimerHandle AI_TriggerMeteorShowerHandle;
	UPROPERTY(EditDefaultsOnly, Category = "AI Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDamageOverTimeData> AI_MeteorShowerData;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> MeteorChargeBallVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlayDeathElectrification() override;
	void AI_TryTriggeringMeteorShower();

	virtual void P_Setup() override;
	virtual void P_WeaponShoot() override;

	virtual void BeginPlay() override;
	virtual void AllDisablingActionsFinished() override;
	virtual void ClearTimers() override;
};
