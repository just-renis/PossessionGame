#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "SpawnLocation.generated.h"

class AEnemy;

UCLASS()
class POSSESSIONGAME_API ASpawnLocation : public AActor
{
	GENERATED_BODY()
public:
	EGameMode GetGameMode() const { return GameMode; }
	TObjectPtr<AEnemy> SpawnEnemy(TSubclassOf<AEnemy> Enemy);
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode GameMode = EGameMode::None;
};