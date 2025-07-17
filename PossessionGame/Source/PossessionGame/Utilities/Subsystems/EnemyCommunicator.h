#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Data Utilities/FEQSPointData.h"
#include "EnemyCommunicator.generated.h"

class APossessionGameGameModeBase;
struct FEnvQueryResult;
class UEnvQuery;
class UDelegateManager;

DECLARE_MULTICAST_DELEGATE(FOnEQSFinish);

UCLASS()
class POSSESSIONGAME_API UEnemyCommunicator : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	bool GetIsUsbDroppable() const { return bIsUsbDroppable; }
	FVector GetPointLocation(ECharacterType InCharacterType, FVector InLocation);
	FVector GetTargetLocation() const;
	TObjectPtr<AActor>& GetTarget() { return Target; }
	FVector GetFarthestEnemyLocation(FVector FromLocation);
	FOnEQSFinish& GetEQSDelegate() { return OnEQSFinish; }
	TArray<TObjectPtr<AActor>>& GetAllEnemies() { return Enemies; }

	void Init(TObjectPtr<UDelegateManager> InDelegateManager, TObjectPtr<UEnvQuery> InTargetEQS);
	void SetTarget(TObjectPtr<AActor> InTarget);
	void AddEnemy(TObjectPtr<AActor> InEnemy);
	void RemoveEnemy(TObjectPtr<AActor> InEnemy);
	void AddPoint(int32 Key, const FEQSPointData& Data);
	void RemovePoint(int32 Key);
	void SetIsUsbDroppable(const bool bState) { bIsUsbDroppable = bState; }
private:
	bool bIsUsbDroppable = false;
	bool bWasTargetSet = false;
	int32 EnemiesAmount = 0;
	FOnEQSFinish OnEQSFinish;
	UPROPERTY()
	FTimerHandle EQSPointsGenerationHandle;
	UPROPERTY()
	TObjectPtr<APossessionGameGameModeBase> GameModeBase;
	UPROPERTY()
	TObjectPtr<UEnvQuery> TargetEQS;
	UPROPERTY()
	TObjectPtr<AActor> Target;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Enemies;
	UPROPERTY()
	TMap<int32, FEQSPointData> PointsAroundTarget;

	void InitDelegates(TObjectPtr<UDelegateManager> InDelegateManager);
	void TargetQueryResult(TSharedPtr<FEnvQueryResult> Result);
	void GeneratePointsAroundTarget();
	virtual void Deinitialize() override;
};
