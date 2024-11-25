#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Minion.generated.h"

UCLASS()
class POSSESSIONGAME_API AMinion : public AEnemy
{
	GENERATED_BODY()
	
	virtual void AI_SetupWeapon() override;

	virtual void BeginPlay() override;
};
