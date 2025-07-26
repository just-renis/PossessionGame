#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Trickster.generated.h"

UCLASS()
class POSSESSIONGAME_API ATrickster : public AEnemy
{
	GENERATED_BODY()
public:
	void P_ThrowPoisonGrenade() const;
	void P_ThrowEmpGrenade() const;
private:
	ATrickster();
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;

	virtual void P_Setup() override;
	virtual void P_SetupColorsForUI() override;

	virtual void BeginPlay() override;
};