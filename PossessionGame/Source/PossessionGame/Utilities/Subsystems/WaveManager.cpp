#include "WaveManager.h"
#include "DelegateManager.h"
#include "PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/SpawnLocation.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/Game Modes/BossModeLogic.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/Game Modes/ChargeTerminalModeLogic.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/Game Modes/HackTerminalModeLogic.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/Game Modes/WaveModeLogic.h"

void UWaveManager::Init(const TObjectPtr<UDelegateManager>& InDelegateManager, const TMap<EGameMode, TObjectPtr<UGameModeData>>& InGameModesData)
{
	DelegateManager = InDelegateManager;
	GameModesData = InGameModesData;
	AssetsManager = GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	DelegateManager->OnGameModeInitiator.AddUObject(this, &UWaveManager::SetAndStartGameMode);
	CurrentGameMode = EGameMode::TutorialMode1;
}

TObjectPtr<UGameModeData> UWaveManager::GetGameModeData(const EGameMode InGameMode)
{
	if (GameModesData.Contains(InGameMode)) return GameModesData[InGameMode];
	return nullptr;
}

void UWaveManager::SetAndStartGameMode(const EGameMode InGameMode)
{
	CurrentGameMode = InGameMode;
	if (CurrentModeLogic) CurrentModeLogic = nullptr;
	switch (InGameMode)
	{
		case EGameMode::WaveMode: CurrentModeLogic = NewObject<UWaveModeLogic>(this); break;
		case EGameMode::HackTerminalsMode: CurrentModeLogic = NewObject<UHackTerminalModeLogic>(this); break;
		case EGameMode::ChargeTerminalMode: CurrentModeLogic = NewObject<UChargeTerminalModeLogic>(this); break;
		case EGameMode::BossMode: CurrentModeLogic = NewObject<UBossModeLogic>(this); break;
		default: return;
	}
	if (!CurrentModeLogic) return;
	CurrentModeLogic->Init(this, GetGameModeData(InGameMode));
}
