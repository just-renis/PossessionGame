#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PyroBossAIController.generated.h"

DECLARE_DELEGATE(FOnStompFinished);
DECLARE_DELEGATE(FOnBallPhaseFinished);

UCLASS()
class POSSESSIONGAME_API APyroBossAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnStompFinished OnStompFinished;
	FOnBallPhaseFinished OnBallPhaseFinished;
};
