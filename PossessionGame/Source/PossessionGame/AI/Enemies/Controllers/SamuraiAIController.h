#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "SamuraiAIController.generated.h"

class ASamurai;

UCLASS()
class POSSESSIONGAME_API ASamuraiAIController : public AEnemyAIController
{
	GENERATED_BODY()

	bool bIsMovingDirectly = false;
	float StuckTimer = 0.f;
	float StuckThreshold = 0.3f;
	FTimerHandle CheckDistanceBetweenTargetHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Slash Data", meta = (AllowPrivateAccess = true))
	float DistanceBeforeSlashingInCentimetres = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Jump Data", meta = (AllowPrivateAccess = true))
	float JumpVelocityForward = 500.f;
	UPROPERTY()
	TObjectPtr<ASamurai> SamuraiProprietor;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;
	virtual void OnLostSightOfPawn() override;
	void SetPlayerTarget(const TObjectPtr<AActor>& InPlayerTarget);
	void CheckDistanceBetweenTarget();
	void CheckIfStuck();
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InEnemy) override;
	virtual void SetIsInJumpMode(const bool bState) override;
};
