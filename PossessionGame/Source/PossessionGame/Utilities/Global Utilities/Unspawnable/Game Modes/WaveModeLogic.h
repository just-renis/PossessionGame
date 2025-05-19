#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "WaveModeLogic.generated.h"

UCLASS()
class POSSESSIONGAME_API UWaveModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()
	
	bool bWaveNotStarted = false;
	int32 CurrentWave;
	int32 EnemiesPerWave = 2;
	int32 AmountOfWaves = 2;
	int32 EnemiesLeftToKill = 2;
	int32 CountdownSeconds = 4;
	
	virtual void Init(const TObjectPtr<UWaveManager> InWaveManager) override;
	virtual void Finalize() override;
	virtual void SetupWaveManagerUI() override;
	virtual void SetupTimers() override;
	virtual void HandleSpawning() override;
	virtual void UpdateEnemiesAmount() override;
	void InitiateNextWave();
	void StartCountdown() const;
	void StartWave() const;
	void SetupNextWave();
	void SetupNextWaveUI() const;
	void FadeOutWavePanel() const;
	void WaveCompleted();
	void WaveModeCallback();
	UFUNCTION()
	void UpdateCountdown();
};
