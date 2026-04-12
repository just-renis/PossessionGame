#include "GameModeBaseLogic.h"
#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UGameModeBaseLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	WaveManager = InWaveManager;
	WaveManager->GetDelegateManager()->RequestCurrentXpNerf.BindUObject(this, &UGameModeBaseLogic::GetCurrentXpNerf);
	EnemiesOnSceneLimit = InGameModeData->GetEnemiesOnSceneLimit();
	SpawnRate = InGameModeData->GetSpawnRate();
	FirstThresholdOfTotalAmountNerf = InGameModeData->GetFirstThresholdOfTotalAmountNerf();
	SecondThresholdOfTotalAmountNerf = InGameModeData->GetSecondThresholdOfTotalAmountNerf();
	WaveManager->GetDelegateManager()->OnPlayerDeath.AddUObject(this, &UGameModeBaseLogic::FadeOutObjectivePanel);
}

float UGameModeBaseLogic::GetCurrentXpNerf() const
{
	if (TotalAmountOfEnemiesKilled >= SecondThresholdOfTotalAmountNerf) return 0.f;
	if (TotalAmountOfEnemiesKilled >= FirstThresholdOfTotalAmountNerf) return 0.5f;
	return 1.f;
}

void UGameModeBaseLogic::SetupWaveManagerUI()
{
	WaveManagerHUD = GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD);
	FadeInObjectivePanel();
}

void UGameModeBaseLogic::Finalize()
{
	WaveManager->GetDelegateManager()->OnGameModeFinished.Broadcast(WaveManager->GetCurrentGameMode());
	FadeOutObjectivePanel();
	WaveManager->GetDelegateManager()->RequestCurrentXpNerf.Unbind();
}

void UGameModeBaseLogic::PlaySound(const ESound InSoundName) const { WaveManager->GetAssetsManager()->PlaySound2D(InSoundName); }

void UGameModeBaseLogic::IncreaseAliveEnemiesAmount()
{
	bAsyncLoadInProcess = false;
	AliveEnemiesAmount++;
}

void UGameModeBaseLogic::FadeOutObjectivePanel() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->PlayObjectivePanelFadeInAnim(false);
}

void UGameModeBaseLogic::FadeInObjectivePanel() const { if (WaveManagerHUD) WaveManagerHUD->PlayObjectivePanelFadeInAnim(true); }

void UGameModeBaseLogic::UpdateEnemiesAmount()
{
	AliveEnemiesAmount--;
	TotalAmountOfEnemiesKilled++;
}
