#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "SoldierBoss.generated.h"

UCLASS()
class POSSESSIONGAME_API ASoldierBoss : public AEnemy
{
	GENERATED_BODY()

	ASoldierBoss();

	virtual void AI_SetupWeapon() override;

	virtual void BeginPlay() override;
};
