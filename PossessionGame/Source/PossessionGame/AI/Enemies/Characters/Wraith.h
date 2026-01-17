#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Wraith.generated.h"

UCLASS()
class POSSESSIONGAME_API AWraith : public AEnemy
{
	GENERATED_BODY()
public:
	void P_InitiateThrowMolotov();
	void P_InitiateThrowTrap();
private:
	AWraith();

	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_TriggerDeathVoiceLine() override;

	virtual void P_Setup() override;
	virtual void P_WeaponShoot() override;

	virtual void BeginPlay() override;
};
