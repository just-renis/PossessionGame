#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "BreacherAIController.generated.h"

DECLARE_DELEGATE(FOnBashFinished);

UCLASS()
class POSSESSIONGAME_API ABreacherAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnBashFinished OnBashFinished;
};
