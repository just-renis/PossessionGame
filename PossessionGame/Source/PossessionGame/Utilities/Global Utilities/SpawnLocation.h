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
	TObjectPtr<AEnemy> SpawnEnemy(TSubclassOf<AEnemy> Enemy);
};