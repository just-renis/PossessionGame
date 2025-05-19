#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameModeBaseLogic.generated.h"

enum class ESound : uint16;
class UWaveManagerHUD;
class UWaveManager;

UCLASS()
class POSSESSIONGAME_API UGameModeBaseLogic : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(const TObjectPtr<UWaveManager> InWaveManager) { WaveManager = InWaveManager; }
protected:
	int32 AliveEnemiesAmount = 0;
	int32 EnemiesOnSceneLimit = 7;
	float SpawnRate = 1.f;
	UPROPERTY()
	TObjectPtr<UWaveManager> WaveManager;
	UPROPERTY()
	TObjectPtr<UWaveManagerHUD> WaveManagerHUD;

	int32 ConvertGameModeToDroneLocation() const;
	
	virtual void SetupWaveManagerUI();
	virtual void SetupTimers() {}
	virtual void Finalize();
	void PlaySound(const ESound InSoundName) const;
	virtual void ResetTaskProgress() const;
	void IncreaseAliveEnemiesAmount() { AliveEnemiesAmount++; }
	UFUNCTION()
	virtual void HandleSpawning() {}
	UFUNCTION()
	virtual void UpdateEnemiesAmount() { AliveEnemiesAmount--; }
	UFUNCTION()
	void ShowUpgradeSelection() const;
	UFUNCTION()
	virtual void UpdateTaskProgress() {}
};
