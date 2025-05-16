#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "RangerAIController.generated.h"

DECLARE_DELEGATE(FOnGrenadeLaunchFinished);
DECLARE_DELEGATE(FOnDeployShieldFinished);

UCLASS()
class POSSESSIONGAME_API ARangerAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnGrenadeLaunchFinished OnGrenadeLaunchFinished;
	FOnDeployShieldFinished OnDeployShieldFinished;
};
