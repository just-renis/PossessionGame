#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/StateTrees/Tasks/TemplateTasks/STTaskMoveToActor.h"
#include "STTaskMoveToEQSLocation.generated.h"

enum class EStateTreeEvent : uint8;

UCLASS(DisplayName = "Move To EQS Location While Shooting", Category = "Hybrid Tasks")
class POSSESSIONGAME_API USTTaskMoveToEQSLocation : public USTTaskMoveToActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsWeaponMontageOnly = false;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void FinishTaskByEvent(const EStateTreeEvent InEvent);
	UFUNCTION()
	void ShootLogic();
};
