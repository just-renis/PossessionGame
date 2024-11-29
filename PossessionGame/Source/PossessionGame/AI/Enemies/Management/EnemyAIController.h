#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy.h"
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

	bool CanExecuteAbility(int32 AbilityIndex) { return Proprietor->CanAbilityBeExecuted(AbilityIndex); }
	TObjectPtr<AEnemy> GetProprietor() { return Proprietor; }
	FStateTreeData& GetStateTreeData() { return StateTreeData; }

	void SetShootCooldown(float Value);
	void SetReloadDelay(float Value);
	virtual void DestroyThis();
	void SetProprietor(TObjectPtr<AEnemy> Enemy);
	void SetupStateTree();
	void SetMoveMode(EMoveMode MoveMode);
	virtual void SetupAI();
	void SetEQSTargetLocation(FVector InLocation) { EQSTarget->SetActorLocation(InLocation); }
protected:
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStateTreeComponent> StateTree;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TObjectPtr<UEnemyCommunicator> EnemyCommunicator;
	
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
private:
	float SightOnTargetRepeat = 0.1f;
	float DistanceBetweenTargetZ = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FStateTreeData StateTreeData;
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> Perception;
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY()
	TObjectPtr<AActor> EQSTarget;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	void CustomFocusToLocation(FVector TargetLocation);
	void SetTarget(TObjectPtr<AActor> Target, bool bIsTargetAcquiredForFirstTime);
	void SetIsTargetInMySight(bool bState);
	void SetupEQSTarget();
	void SetEQSTargetLocation();
	void StopStateTreeLogic();

	UFUNCTION()
	void OnLostSightOfPawn();
	UFUNCTION()
	void SightOnTargetLogic();
	UFUNCTION()
	void AdjustCustomFocus();
};