#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "TricksterAIController.generated.h"

DECLARE_DELEGATE(FOnPoisonGrenadeLaunched);

UCLASS()
class POSSESSIONGAME_API ATricksterAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnPoisonGrenadeLaunched OnPoisonGrenadeLaunched;
};