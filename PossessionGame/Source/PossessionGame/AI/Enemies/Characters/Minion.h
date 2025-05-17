#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Minion.generated.h"

UCLASS()
class POSSESSIONGAME_API AMinion : public AEnemy
{
	GENERATED_BODY()

	AMinion();
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;

	virtual void BeginPlay() override;
	virtual void DestroyAI() override;
};
