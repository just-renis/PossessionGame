#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "MinionBossAIController.generated.h"

DECLARE_DELEGATE(FOnStompFinished);
DECLARE_DELEGATE(FOnLaserEyeFinished);

UCLASS()
class POSSESSIONGAME_API AMinionBossAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnStompFinished OnStompFinished;
	FOnLaserEyeFinished OnLaserEyeFinished;
};
