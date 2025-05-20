#include "GameModeBaseLogic.h"

#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UGameModeBaseLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	WaveManager = InWaveManager;
	AliveEnemiesAmount = InGameModeData->GetAliveEnemiesAmount();
	EnemiesOnSceneLimit = InGameModeData->GetEnemiesOnSceneLimit();
	SpawnRate = InGameModeData->GetSpawnRate();
}

int32 UGameModeBaseLogic::ConvertGameModeToDroneLocation() const
{
	switch (WaveManager->GetCurrentGameMode())
	{
		case EGameMode::WaveMode: return 3;
		case EGameMode::HackTerminalsMode: return 4;
		case EGameMode::ChargeTerminalMode: return 5;
		default: ;
	}
	return 0;
}

void UGameModeBaseLogic::SetupWaveManagerUI() { WaveManagerHUD = GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD); }

void UGameModeBaseLogic::Finalize()
{
	WaveManager->GetDelegateManager()->OnGameModeFinished.Broadcast(WaveManager->GetCurrentGameMode());
	(void)WaveManager->GetDelegateManager()->StartTutorialDroneMoveToLocation.ExecuteIfBound(ConvertGameModeToDroneLocation(), true);
}

void UGameModeBaseLogic::PlaySound(const ESound InSoundName) const { WaveManager->GetAssetsManager()->PlaySound2D(InSoundName); }

void UGameModeBaseLogic::ResetTaskProgress() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->ToggleTaskProgress(false, WaveManager->GetCurrentGameMode());
	WaveManagerHUD->UpdateTaskProgress(0.f, 1.f);
}

void UGameModeBaseLogic::ShowUpgradeSelection() const { GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->OpenUI(); }
