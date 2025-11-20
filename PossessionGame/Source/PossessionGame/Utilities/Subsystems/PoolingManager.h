#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolingManager.generated.h"

class ABreacherDynamite;
class ARangerMine;
enum class EPoolObject : uint8;
class APoolObject;

USTRUCT()
struct POSSESSIONGAME_API FObjectPoolData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<APoolObject> ObjectClass;
	UPROPERTY()
	TArray<TObjectPtr<APoolObject>> Objects;
};

UCLASS()
class POSSESSIONGAME_API UPoolingManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	bool CanRangerThrowMine();
	bool CanBreacherThrowDynamite();
	TObjectPtr<APoolObject> SpawnObjectFromPool(const EPoolObject InObjectName, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator);
	TObjectPtr<APoolObject> SpawnRangerMineFromPool(const FVector& InLocation, const FRotator& InRotation);
	TObjectPtr<APoolObject> SpawnBreacherDynamiteFromPool(const FVector& InLocation, const FRotator& InRotation);
	
	void Init(TMap<EPoolObject, TSubclassOf<APoolObject>>& InPoolMap);
	void ReturnObjectToPool(const EPoolObject InObjectName, const TObjectPtr<APoolObject>& InPoolObject);
	void ReturnRangerMineToPool(const TObjectPtr<APoolObject>& InPoolObject);
	void ReturnBreacherDynamiteToPool(const TObjectPtr<APoolObject>& InPoolObject);
private:
	int32 RangerMineIndexToUse = -1;
	int32 DefaultRangerMineAmountCap = 3;
	int32 RangerMineAmountCap = 3;
	int32 BreacherDynamiteIndexToUse = -1;
	int32 DefaultBreacherDynamiteAmountCap = 1;
	int32 BreacherDynamiteAmountCap = 1;
	UPROPERTY()
	TMap<EPoolObject, FObjectPoolData> Pool;
	UPROPERTY()
	TArray<TObjectPtr<ARangerMine>> RangerExistingMines;
	UPROPERTY()
	TArray<TObjectPtr<ABreacherDynamite>> BreacherExistingDynamites;

	void AllocatePoolObject(const EPoolObject InObjectName, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator);
	void SpawnDamageNumber(const FLinearColor InColor, const float InDamage, const FVector& InLocation, const FVector& InForwardActor);
	void RemoveRangerMineFromExistingMines(const TObjectPtr<ARangerMine>& InRangerMine) { RangerExistingMines.Remove(InRangerMine); }
	void RemoveBreacherDynamiteFromExistingDynamites(const TObjectPtr<ABreacherDynamite>& InBreacherDynamite) { BreacherExistingDynamites.Remove(InBreacherDynamite); }
};
