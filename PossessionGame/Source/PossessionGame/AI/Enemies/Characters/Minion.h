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
	UFUNCTION()
	void AI_PlaySpawnSound();

	virtual void BeginPlay() override;
	virtual void DestroyAI() override;
};
