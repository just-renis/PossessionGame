#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "WraithAIController.generated.h"

DECLARE_DELEGATE(FOnMeteorShowerFinished);

UCLASS()
class POSSESSIONGAME_API AWraithAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnMeteorShowerFinished OnMeteorShowerFinished;
};
