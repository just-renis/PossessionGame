#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PyroBossAIController.generated.h"

DECLARE_DELEGATE(FOnActivateFireFloorFinished);

UCLASS()
class POSSESSIONGAME_API APyroBossAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnActivateFireFloorFinished OnActivateFireFloorFinished;
};
