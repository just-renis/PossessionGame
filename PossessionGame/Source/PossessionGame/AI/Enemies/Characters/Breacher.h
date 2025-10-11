#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Breacher.generated.h"

UCLASS()
class POSSESSIONGAME_API ABreacher : public AEnemy
{
	GENERATED_BODY()

	virtual void AI_SetupWeapon() override;

	virtual void P_Setup() override;

	virtual void BeginPlay() override;
};
