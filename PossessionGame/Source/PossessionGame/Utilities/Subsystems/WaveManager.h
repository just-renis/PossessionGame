#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "WaveManager.generated.h"

class UDelegateManager;
class UPrimaryDataAssetsManager;
class ASpawnLocation;
class AEnemy;
class UWaveManagerHUD;
enum class ESound : uint8;
enum class EWeapon : uint8;

USTRUCT()
struct POSSESSIONGAME_API FGameModeSpawnLocations
{
	GENERATED_BODY()

	TArray<TObjectPtr<ASpawnLocation>> SpawnLocations;
};

UCLASS()
class POSSESSIONGAME_API UWaveManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	void Init(const bool bIsTutorialFinished, const TObjectPtr<UDelegateManager>& InDelegateManager);
	void SetupWaveManagerSettings(const TArray<ECharacterType>& InEnemiesThatAreSpawnable, const int32 InEnemiesAmountOnFirstWave);
private:
	bool bIsSpawnInProcess = false;
	bool bWaveNotStarted = false;
	bool bTutorialIsFinished = false;
	bool bUnlimitedSpawning = false;
	int32 CurrentWave = 1;
	int32 EnemiesPerWave = 2;
	int32 EnemiesLeftToKill = 2;
	int32 EnemiesOnSceneLimit = 7;
	int32 AliveEnemiesAmount = 0;
	int32 CountdownSeconds = 4;
	int32 TerminalsLeftToHack = 2;
	float SpawnRate = 1.f;
	float TerminalHackTimer = 0.f;
	float TerminalHackRequired = 30.f;
	EGameMode CurrentGameMode = EGameMode::None;
	UPROPERTY()
	TObjectPtr<UWaveManagerHUD> WaveManagerHUD;
	UPROPERTY()
	TMap<EGameMode, FGameModeSpawnLocations> SpawnLocations;
	UPROPERTY()
	TArray<ECharacterType> EnemyTypes;
	UPROPERTY()
	TMap<ECharacterType, TSubclassOf<AEnemy>> Enemies;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	bool IsGameRunning();
	EWeapon GetWeaponCode(ECharacterType EnemyType);
	EWeapon GetSingularWeapon(EWeapon Weapon);

	void InitMode();
	void FinalizeMode();
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
	void SetAndStartGameMode(const EGameMode InGameMode);
	void FinishTutorial();
	void StartTerminalHack();
	void FinishTerminalHack();

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
	UFUNCTION()
	void UpdateTaskProgress();
};