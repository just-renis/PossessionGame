#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "SamuraiAIController.generated.h"

UCLASS()
class POSSESSIONGAME_API ASamuraiAIController : public AEnemyAIController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
};
