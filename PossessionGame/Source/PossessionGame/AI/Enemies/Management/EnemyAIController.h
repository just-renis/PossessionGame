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

UCLASS()
class POSSESSIONGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyAIController();

	bool CanExecuteAbility(const int32 InAbilityIndex) const { return Proprietor->CanAbilityBeExecuted(InAbilityIndex); }
	EStateTreeEvent GetCurrentStateTreeEvent() const { return CurrentStateTreeEvent; }
	TObjectPtr<AEnemy> GetProprietor() { return Proprietor; }
	FStateTreeData& GetStateTreeData() { return StateTreeData; }

	void SetShootCooldown(const float InValue);
	void SetReloadDelay(const float InValue);
	virtual void DestroyThis();
	void SetProprietor(const TObjectPtr<AEnemy>& InEnemy);
	void SetupStateTree();
	void SetMoveMode(const EMoveMode InMoveMode) const;
	virtual void SetupAI();
	void SetEQSTargetLocation(const FVector& InLocation) const { EQSTarget->SetActorLocation(InLocation); }
	void ResumeLogic() const;
	void StopLogic(const bool bNeedCleanup);
	void SetStateTreeCurrentEvent(const EStateTreeEvent InCurrentStateTreeEvent) { CurrentStateTreeEvent = InCurrentStateTreeEvent; }
protected:
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStateTreeComponent> StateTree;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TObjectPtr<UEnemyCommunicator> EnemyCommunicator;
	
	virtual void BeginPlay() override;
	virtual void AdjustCustomFocus() const;
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
private:
	EStateTreeEvent CurrentStateTreeEvent;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FStateTreeData StateTreeData;
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> Perception;
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY()
	TObjectPtr<AActor> EQSTarget;
	UPROPERTY()
	FTimerHandle AdjustCustomFocusHandle;
	UPROPERTY()
	FTimerHandle OnLostSightOfPawnHandle;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	void CustomFocusToLocation(const FVector& InTargetLocation) const;
	void SetTarget(const TObjectPtr<AActor>& InTarget, const bool bIsTargetAcquiredForFirstTime);
	void SetIsTargetInMySight(const bool bState);
	void SetupEQSTarget();
	void SetEQSTargetLocation();
	void StopStateTreeLogic() const;
	void StopStateTreeLogic(const bool bNeedCleanup) const;
	void OnLostSightOfPawn();
};