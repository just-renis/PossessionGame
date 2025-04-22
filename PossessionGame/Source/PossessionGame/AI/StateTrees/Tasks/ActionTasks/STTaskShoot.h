#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskShoot.generated.h"

class AEnemyAIController;

UCLASS(DisplayName = "Shoot At Player", Category = "Action Tasks")
class POSSESSIONGAME_API USTTaskShoot : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsWeaponMontageOnly = false;
	UPROPERTY()
	TObjectPtr<AEnemyAIController> CachedController;
	UPROPERTY()
	FTimerHandle ShootLogicHandle;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	UFUNCTION()
	void ShootLogic();
};
