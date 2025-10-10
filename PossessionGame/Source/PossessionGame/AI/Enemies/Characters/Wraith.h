#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Wraith.generated.h"

UCLASS()
class POSSESSIONGAME_API AWraith : public AEnemy
{
	GENERATED_BODY()

	virtual void AI_SetupWeapon() override;

	virtual void BeginPlay() override;
};
