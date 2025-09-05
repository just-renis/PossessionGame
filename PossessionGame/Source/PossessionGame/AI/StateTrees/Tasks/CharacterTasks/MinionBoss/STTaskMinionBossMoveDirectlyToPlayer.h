#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/StateTrees/Tasks/TemplateTasks/STTaskMoveToActor.h"
#include "STTaskMinionBossMoveDirectlyToPlayer.generated.h"

UCLASS(DisplayName = "Move Directly To Player", Category = "Character Tasks | Minion Boss Tasks")
class POSSESSIONGAME_API USTTaskMinionBossMoveDirectlyToPlayer : public USTTaskMoveToActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float MoveDirectlyToPlayerDuration = 5.f;
	UPROPERTY(EditAnywhere, Category = "Individual Values | Movement", meta = (AllowPrivateAccess = true))
	float MoveSpeedMultiplier = 2.f;
	UPROPERTY()
	FTimerHandle MoveDirectlyToPlayerHandle;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	void OnMoveDirectlyDurationFinished();
};
