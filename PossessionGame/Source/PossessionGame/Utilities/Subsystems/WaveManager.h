#pragma once

#include "CoreMinimal.h"
#include "WaveManager.generated.h"

enum class EGameMode : uint8;
class UDelegateManager;
class UPrimaryDataAssetsManager;
class UGameModeBaseLogic;

UCLASS()
class POSSESSIONGAME_API UWaveManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	EGameMode GetCurrentGameMode() const { return CurrentGameMode; }
	TObjectPtr<UDelegateManager> GetDelegateManager() const { return DelegateManager; }
	TObjectPtr<UPrimaryDataAssetsManager> GetAssetsManager() const { return AssetsManager; }
	
	void SetCurrentGameMode(const EGameMode InGameMode) { CurrentGameMode = InGameMode; }
	void Init(const TObjectPtr<UDelegateManager>& InDelegateManager);
private:
	EGameMode CurrentGameMode;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TObjectPtr<UGameModeBaseLogic> CurrentModeLogic = nullptr;

	void SetAndStartGameMode(const EGameMode InGameMode);
	//void FinishTutorial();
};