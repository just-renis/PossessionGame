#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "WaveModeData.generated.h"

UCLASS()
class POSSESSIONGAME_API UWaveModeData : public UGameModeData
{
	GENERATED_BODY()
public:
	int32 GetEnemiesPerWave() const { return EnemiesPerWave; }
	int32 GetEnemiesAmountIncreasePerWave() const { return EnemiesAmountIncreasePerWave; }
	int32 GetAmountOfWaves() const { return AmountOfWaves; }
	int32 GetCountdownSeconds() const { return CountdownSeconds; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 EnemiesPerWave = 2;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 EnemiesAmountIncreasePerWave;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 AmountOfWaves = 2;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 CountdownSeconds = 4;
};
