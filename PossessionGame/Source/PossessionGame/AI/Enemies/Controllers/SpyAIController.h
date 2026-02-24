#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "SpyAIController.generated.h"

DECLARE_DELEGATE(FOnMovementToBubbleFinished);

UCLASS()
class POSSESSIONGAME_API ASpyAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnMovementToBubbleFinished OnMovementToBubbleFinished;
};