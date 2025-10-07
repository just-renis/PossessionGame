#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "ArenaModeLogic.generated.h"

class UArenaModeData;

UCLASS()
class POSSESSIONGAME_API UArenaModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()

	bool bWaveNotStarted = false;
	int32 CurrentWave = 1;
	int32 EnemiesPerWave;
	int32 EnemiesLeftToKill;
	int32 CountdownSeconds;
	UPROPERTY()
	FTimerHandle CountdownHandle;
	UPROPERTY()
	FTimerHandle OpenUpgradeSelectionHandle;
	UPROPERTY()
	TObjectPtr<UArenaModeData> Data;

	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	void SetData();
	void InitiateNextWave();
	void SetupNextWave();
	void UpdateCountdown();
	void StartWave();
	void StartCountdown() const;
	virtual void SetupTimers() override;
	virtual void SetupWaveManagerUI() override;
	virtual void HandleSpawning() override;
	void WaveCompleted();
	void AfterSpawnEnemyCallback();
	virtual void FadeInObjectivePanel() const override;
};
