#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameModeBaseLogic.generated.h"

class UGameModeData;
enum class ESound : uint16;
class UWaveManagerHUD;
class UWaveManager;

UCLASS()
class POSSESSIONGAME_API UGameModeBaseLogic : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData);
protected:
	bool bInstanceFinished = false;
	bool bAsyncLoadInProcess = false;
	int32 AliveEnemiesAmount = 0;
	int32 EnemiesOnSceneLimit;
	float SpawnRate;
	UPROPERTY()
	TObjectPtr<UWaveManager> WaveManager;
	UPROPERTY()
	TObjectPtr<UWaveManagerHUD> WaveManagerHUD;
	UPROPERTY()
	FTimerHandle SpawnHandle;

	int32 ConvertGameModeToDroneLocation() const;
	
	virtual void SetupWaveManagerUI();
	virtual void SetupTimers() {}
	virtual void Finalize();
	void PlaySound(const ESound InSoundName) const;
	void IncreaseAliveEnemiesAmount();
	void FadeOutObjectivePanel() const;
	virtual void HandleSpawning() {}
	virtual void FadeInObjectivePanel() const;
	virtual void UpdateEnemiesAmount() { AliveEnemiesAmount--; }
	void ShowUpgradeSelection() const;
	virtual void UpdateTaskProgress() {}
};
