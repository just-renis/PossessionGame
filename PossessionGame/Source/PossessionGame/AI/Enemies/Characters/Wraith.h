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
private:
	virtual void AI_SetupWeapon() override;

	virtual void P_Setup() override;
	virtual void P_WeaponShoot() override;

	virtual void BeginPlay() override;
};
