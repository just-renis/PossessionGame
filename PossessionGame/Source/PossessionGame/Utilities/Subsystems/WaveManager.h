#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "WaveManager.generated.h"

class UDelegateManager;
class UPrimaryDataAssetsManager;
class ASpawnLocation;
class AEnemy;
class UWaveManagerHUD;
enum class ESound : uint8;
enum class EWeapon : uint8;

UCLASS()
class POSSESSIONGAME_API UWaveManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	void Init(const bool bIsTutorialFinished, const TObjectPtr<UDelegateManager>& InDelegateManager);
	void InitDelegates();
	void SetupWaveManagerSettings(TArray<ECharacterType>& EnemiesThatAreSpawnable, ECharacterType FirstEnemy, int32 EnemiesAmountOnFirstWave);
private:
	bool FIRSTPOSSESSABLE = true;
	bool bIsSpawnInProcess = false;
	bool bWaveNotStarted = false;
	bool bTutorialIsFinished = false;
	int32 CurrentWave = 1;
	int32 EnemiesPerWave = 2;
	int32 EnemiesLeftToKill = 2;
	int32 EnemiesOnSceneLimit = 7;
	int32 AliveEnemiesAmount = 0;
	int32 CountdownSeconds = 4;
	float SpawnRate = 0.2f;
	ECharacterType FirstEnemyToSpawn = ECharacterType::Spy;
	UPROPERTY()
	TObjectPtr<UWaveManagerHUD> WaveManagerHUD;
	UPROPERTY()
	TArray<TObjectPtr<ASpawnLocation>> SpawnLocations;
	UPROPERTY()
	TArray<ECharacterType> EnemyTypes;
	UPROPERTY()
	TMap<ECharacterType, TSubclassOf<AEnemy>> Enemies;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	bool IsGameRunning();
	EWeapon GetWeaponCode(ECharacterType EnemyType);
	EWeapon GetSingularWeapon(EWeapon Weapon);

	void SetupTimers();
	void StartWave();
	void InitiateNextWave();
	void SetupNextWave();
	void SetupNextWaveUI();
	void SetupWaveManagerUI();
	void SetupSpawnLocations();
	void SpawnEnemy(const ECharacterType InEnemyType);
	void SpawnLogic();
	void AsyncLoadAndSpawnEnemy(ECharacterType EnemyType);
	void PlaySound(const ESound InSoundName) const;
	void AsyncLoadCallback(EWeapon SingularWeapon, EWeapon Weapon, ECharacterType EnemyType, TObjectPtr<UPrimaryDataAssetsManager> AssetsManage);
	void FadeOutWavePanel() const;
	void WaveCompleted();
	void TutorialHandleSpawning(const ECharacterType InEnemyType);

	UFUNCTION()
	void ShowUpgradeSelection();
	UFUNCTION()
	void UpdateCountdown();
	UFUNCTION()
	void HandleSpawning();
	UFUNCTION()
	void InitiateWave();
	UFUNCTION()
	void UpdateEnemiesAmount();
};