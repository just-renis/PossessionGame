#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "SamuraiAIController.generated.h"

UCLASS()
class POSSESSIONGAME_API ASamuraiAIController : public AEnemyAIController
{
	GENERATED_BODY()

	bool bIsMovingDirectly = false;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;
	virtual void OnLostSightOfPawn() override;
	void SetPlayerTarget(const TObjectPtr<AActor>& InPlayerTarget);
};
