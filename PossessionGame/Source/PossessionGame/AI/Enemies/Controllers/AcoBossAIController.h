#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "AcoBossAIController.generated.h"

class AAcoBoss;

UCLASS()
class POSSESSIONGAME_API AAcoBossAIController : public AEnemyAIController
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AAcoBoss> RealProprietor;

	virtual FVector GetFocusTargetLocation() const override;

	virtual void BeginPlay() override;
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InEnemy) override;
};
