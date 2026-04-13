#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskSoldierBossMissilesLaunch.generated.h"

class ASoldierBossAIController;

UCLASS(DisplayName = "Missiles Launch", Category = "Character Tasks | Soldier Boss Tasks")
class POSSESSIONGAME_API USTTaskSoldierBossMissilesLaunch : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
    TObjectPtr<ASoldierBossAIController> CachedController;

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
    void MissilesLaunchFinished();
};
