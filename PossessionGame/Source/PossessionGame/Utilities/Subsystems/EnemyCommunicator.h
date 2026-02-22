#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Data Utilities/FEQSPointData.h"
#include "EnemyCommunicator.generated.h"

enum class EGameMode : uint8;
class ATeleportBubble;
class AEnemy;
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
	bool GetIsEnemiesAvailable() const { return !Enemies.IsEmpty(); }
	FVector GetPointLocation(ECharacterType InCharacterType, FVector InLocation);
	FVector GetTargetLocation() const;
	TObjectPtr<AEnemy>& GetTarget() { return Target; }
	FVector GetFarthestEnemyLocation(const FVector& InFromLocation);
	TObjectPtr<AActor> GetClosestEnemy(const FVector& InFromLocation, const TObjectPtr<AActor>& InIgnoreEnemy = nullptr);
	FOnEQSFinish& GetEQSDelegate() { return OnEQSFinish; }
	TArray<TObjectPtr<AActor>>& GetAllEnemies() { return Enemies; }

	void Init(TObjectPtr<UDelegateManager> InDelegateManager, TObjectPtr<UEnvQuery> InTargetEQS);
	void SetTarget(const TObjectPtr<AEnemy>& InTarget);
	void AddEnemy(const TObjectPtr<AActor>& InEnemy);
	void RemoveEnemy(const TObjectPtr<AActor>& InEnemy);
	void AddPoint(const int32 InKey, const FEQSPointData& InData);
	void RemovePoint(const int32 InKey);
	void AddTeleportBubble(const TObjectPtr<ATeleportBubble>& InTeleportBubble) { TeleportBubbles.Add(InTeleportBubble); }
private:
	bool bWasTargetSet = false;
	int32 EnemiesAmount = 0;
	FTimerHandle EQSPointsGenerationHandle;
	FOnEQSFinish OnEQSFinish;
	UPROPERTY()
	TObjectPtr<APossessionGameGameModeBase> GameModeBase;
	UPROPERTY()
	TObjectPtr<UEnvQuery> TargetEQS;
	UPROPERTY()
	TObjectPtr<AEnemy> Target;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Enemies;
	UPROPERTY()
	TArray<TObjectPtr<ATeleportBubble>> TeleportBubbles;
	UPROPERTY()
	TMap<int32, FEQSPointData> PointsAroundTarget;

	void InitDelegates(const TObjectPtr<UDelegateManager>& InDelegateManager);
	void TargetQueryResult(TSharedPtr<FEnvQueryResult> Result);
	void GeneratePointsAroundTarget();
	virtual void Deinitialize() override;
	void TeleportToBubble(const TObjectPtr<AActor>& InActor);
};
