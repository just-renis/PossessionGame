#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "SoldierBossAIController.generated.h"

DECLARE_DELEGATE(FOnMissilesLaunchFinished);

UCLASS()
class POSSESSIONGAME_API ASoldierBossAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnMissilesLaunchFinished OnMissilesLaunchFinished;
};
