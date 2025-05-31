#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskShoot.generated.h"

enum class EStateTreeEvent : uint8;
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
	void FinishTaskByEvent(const EStateTreeEvent InEvent);
	void ShootLogic();
};
