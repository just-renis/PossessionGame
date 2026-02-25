#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Utilities/Data Utilities/FStateTreeData.h"
#include "EnemyAIController.generated.h"

class UStateTreeComponent;
enum class EMoveMode : uint8;
struct FAIStimulus;
class UAISenseConfig_Sight;
class AAIPath;
class UEnemyCommunicator;
class AEnemy;

DECLARE_DELEGATE(FOnEqsTargetUpdate);

UCLASS()
class POSSESSIONGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyAIController();

	bool CanExecuteAbility(const int32 InAbilityIndex) const { return Proprietor->CanAbilityBeExecuted(InAbilityIndex); }
	bool GetIsUsingEQS() const { return bIsUsingEQS; }
	EStateTreeEvent GetCurrentStateTreeEvent() const { return CurrentStateTreeEvent; }
	TObjectPtr<AEnemy> GetProprietor() { return Proprietor; }
	FStateTreeData& GetStateTreeData() { return StateTreeData; }
	FOnEqsTargetUpdate& GetEqsTargetUpdateDelegate() { return OnEqsTargetUpdate; }

	void SetStateTreeCurrentEvent(const EStateTreeEvent InCurrentStateTreeEvent) { CurrentStateTreeEvent = InCurrentStateTreeEvent; }
	void SetControlRigAvailabilityToggle(const bool bState) { bIsControlRigAvailableToToggle = bState; }
	void SetEQSTargetLocation(const FVector& InLocation) const { EQSTarget->SetActorLocation(InLocation); }
	void SetIsUsingEQS(const bool bState) { bIsUsingEQS = bState; }
	void SetCustomFocusState(const bool bState) { bCustomFocusEnabled = bState; }
	void SetShootCooldown(const float InValue);
	void SetReloadDelay(const float InValue);
	virtual void DestroyThis();
	void SetProprietor(const TObjectPtr<AEnemy>& InEnemy);
	void SetupStateTree();
	void SetMoveMode(const EMoveMode InMoveMode) const;
	virtual void SetupAI();
	void ResumeLogic() const;
	void StopLogic(const bool bNeedCleanup);
	void SetTargetLocation(const FVector& InLocation);
	virtual void ResetOnReachedNonEqsPoint() {}
protected:
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStateTreeComponent> StateTree;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TObjectPtr<UEnemyCommunicator> EnemyCommunicator;
	
	virtual void BeginPlay() override;
	virtual void AdjustCustomFocus();
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
private:
	bool bIsControlRigAvailableToToggle = true;
	bool bIsUsingEQS = true;
	FTimerHandle AdjustCustomFocusHandle;
	FTimerHandle OnLostSightOfPawnHandle;
	EStateTreeEvent CurrentStateTreeEvent;
	FOnEqsTargetUpdate OnEqsTargetUpdate;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	bool bPredictionEnabled = false;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	bool bPredictionClampEnabled = false;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	float PredictionTimeMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	float PredictionStartsAt = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	float PredictionMaxClamp = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	float DistanceScalingMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	float DistanceScalingMultiplierStart = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	float ProjectileSpeed = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	float VerticalDeltaThreshold = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Prediction", meta = (AllowPrivateAccess = true))
	float VerticalDeltaAddition = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Custom Focus", meta = (AllowPrivateAccess = true))
	bool bCustomFocusEnabled = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FStateTreeData StateTreeData;
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> Perception;
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY()
	TObjectPtr<AActor> EQSTarget;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	void CustomFocusToLocation(const FVector& InTargetLocation) const;
	void SetTarget(const TObjectPtr<AEnemy>& InTarget, const bool bIsTargetAcquiredForFirstTime);
	void SetIsTargetInMySight(const bool bState);
	void SetupEQSTarget();
	void SetEQSTargetLocation();
	void StopStateTreeLogic() const;
	void StopStateTreeLogic(const bool bNeedCleanup) const;
	void OnLostSightOfPawn();
};