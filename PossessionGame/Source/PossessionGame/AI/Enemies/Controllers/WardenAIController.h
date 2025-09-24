#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "WardenAIController.generated.h"

DECLARE_DELEGATE(FOnBeamShot);

UCLASS()
class POSSESSIONGAME_API AWardenAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
	FOnBeamShot OnBeamShot;
};
