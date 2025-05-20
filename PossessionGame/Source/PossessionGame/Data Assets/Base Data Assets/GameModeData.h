#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameModeData.generated.h"

UCLASS()
class POSSESSIONGAME_API UGameModeData : public UDataAsset
{
	GENERATED_BODY()
public:
	int32 GetEnemiesOnSceneLimit() const { return EnemiesOnSceneLimit; }
	float GetSpawnRate() const { return SpawnRate; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 EnemiesOnSceneLimit = 7;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float SpawnRate = 1.f;
};
