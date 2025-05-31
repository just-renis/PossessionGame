#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "STTaskMoveToActor.generated.h"

enum class EMoveMode : uint8;
class UNavigationQueryFilter;
struct FAIMoveRequest;
class AEnemyAIController;
namespace EPathFollowingRequestResult { enum Type : int; }
struct FPathFollowingResult;
struct FAIRequestID;

UCLASS(DisplayName = "Move To", Category = "Template Tasks")
class POSSESSIONGAME_API USTTaskMoveToActor : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<AEnemyAIController> CachedController;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bFinishTaskOnMoveCompleted = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bMoveToTargetLocation = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bPathFinding = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bAllowPartial = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bStrafe = false;
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (AllowPrivateAccess = true))
	float AcceptableRadius = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EMoveMode MoveMode = EMoveMode::Walk;
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (AllowPrivateAccess = true))
	FVector TargetLocation = FVector(NAN, NAN, NAN);
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> Target = nullptr;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UNavigationQueryFilter> FilterClass;
	UPROPERTY()
	FTimerHandle CheckForNewMoveReqHandle;

	static EStateTreeRunStatus PathFollowingRequestResultHandling(EPathFollowingRequestResult::Type MoveResult);
	FAIMoveRequest SetupAndGetMoveReq();
	
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	void CheckForNewMoveReq();
};
