#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "WaveModeLogic.generated.h"

class UWaveModeData;

UCLASS()
class POSSESSIONGAME_API UWaveModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()

	bool bWaveNotStarted = false;
	int32 CurrentWave = 1;
	int32 EnemiesPerWave;
	int32 AmountOfWaves;
	int32 EnemiesLeftToKill;
	int32 CountdownSeconds;
	UPROPERTY()
	TObjectPtr<UWaveModeData> Data;
	UPROPERTY()
	FTimerHandle OpenUpgradeSelectionHandle;
	UPROPERTY()
	FTimerHandle CountdownHandle;
	
	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	virtual void Finalize() override;
	virtual void SetupWaveManagerUI() override;
	virtual void SetupTimers() override;
	virtual void HandleSpawning() override;
	virtual void FadeInObjectivePanel() const override;
	void InitiateNextWave();
	void StartCountdown() const;
	void StartWave();
	void SetupNextWave();
	void WaveCompleted();
	void AfterSpawnEnemyCallback();
	void SetData();
	void UpdateCountdown();
};
