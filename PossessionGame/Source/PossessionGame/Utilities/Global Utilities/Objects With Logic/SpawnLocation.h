#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnLocation.generated.h"

class AEnemy;

UCLASS()
class POSSESSIONGAME_API ASpawnLocation : public AActor
{
	GENERATED_BODY()
public:
	int32 GetOrder() const { return Order; }
	TObjectPtr<AEnemy> SpawnEnemy(TSubclassOf<AEnemy> Enemy);
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Order = 0;
};