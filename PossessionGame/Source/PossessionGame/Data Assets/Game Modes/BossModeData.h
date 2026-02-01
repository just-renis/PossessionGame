#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "BossModeData.generated.h"

UCLASS()
class POSSESSIONGAME_API UBossModeData : public UGameModeData
{
	GENERATED_BODY()
public:
	ECharacterType GetEnemyToSpawn() const { return EnemyToSpawn; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	ECharacterType EnemyToSpawn;
};
