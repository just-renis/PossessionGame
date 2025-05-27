#include "WaveModeLogic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Data Assets/Game Modes/WaveModeData.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UWaveModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	Data = Cast<UWaveModeData>(InGameModeData);
	SetData();
	WaveManager->GetDelegateManager()->OnUpgradeSelectionFinished.BindUObject(this, &UWaveModeLogic::InitiateNextWave);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UWaveModeLogic::UpdateEnemiesAmount);
	SetupWaveManagerUI();
	SetupTimers();
}

void UWaveModeLogic::Finalize()
{
	WaveManager->GetDelegateManager()->OnUpgradeSelectionFinished.Unbind();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	Super::Finalize();
}

void UWaveModeLogic::SetupWaveManagerUI()
{
	Super::SetupWaveManagerUI();
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateCountdown(CountdownSeconds);
	FadeOutObjectivePanel();
	if (WaveManagerHUD->GetIsObjectivePanelFadeInAtLeastOnce()) UKismetSystemLibrary::K2_SetTimer(this, "FadeInObjectivePanel", WaveManagerHUD->GetObjectivePanelFadeInEndTime(), false);
	else FadeInObjectivePanel();
}

void UWaveModeLogic::SetupTimers()
{
	UpdateCountdown();
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateCountdown", 1.f, true);
}

void UWaveModeLogic::HandleSpawning()
{
	if (EnemiesLeftToKill > 0 && AliveEnemiesAmount < EnemiesOnSceneLimit) WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(), true, [this] { AfterSpawnEnemyCallback(); });
	else if (!bWaveNotStarted && EnemiesLeftToKill <= 0 && AliveEnemiesAmount <= 0) WaveCompleted();
}

void UWaveModeLogic::FadeInObjectivePanel() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjective(EGameMode::WaveMode, AmountOfWaves);
	WaveManagerHUD->UpdateObjectiveProgress(0, AmountOfWaves);
	Super::FadeInObjectivePanel();
}

void UWaveModeLogic::InitiateNextWave()
{
	if (CurrentWave++ < AmountOfWaves)
	{
		bWaveNotStarted = false;
		SetupNextWave();
		if (WaveManagerHUD) StartCountdown();
	}
	else Finalize();
}

void UWaveModeLogic::StartCountdown() const { if (WaveManagerHUD) WaveManagerHUD->ToggleCountdownText(); }

void UWaveModeLogic::StartWave() const
{
	PlaySound(ESound::WaveStart);
	PlaySound(ESound::TaskAnnouncement);
	if (!WaveManagerHUD) return;
	WaveManagerHUD->ToggleCountdownText();
}

void UWaveModeLogic::SetupNextWave()
{
	EnemiesPerWave += 2;
	EnemiesLeftToKill = EnemiesPerWave;
	CountdownSeconds = 4.f;
	UpdateCountdown();
	UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateCountdown", 1.f, true);
}

void UWaveModeLogic::WaveCompleted()
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjectiveProgress(CurrentWave, AmountOfWaves);
	WaveManagerHUD->PlayAnnouncementAnim("Wave Completed");
	PlaySound(ESound::WaveCompleted);
	bWaveNotStarted = true;
	UKismetSystemLibrary::K2_SetTimer(this, "ShowUpgradeSelection", WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
}

void UWaveModeLogic::AfterSpawnEnemyCallback()
{
	AliveEnemiesAmount++;
	EnemiesLeftToKill--;
}

void UWaveModeLogic::SetData()
{
	EnemiesPerWave = Data->GetEnemiesPerWave();
	AmountOfWaves = Data->GetAmountOfWaves();
	EnemiesLeftToKill = Data->GetEnemiesLeftToKill();
	CountdownSeconds = Data->GetCountdownSeconds();
}

void UWaveModeLogic::UpdateCountdown()
{
	if (WaveManagerHUD)
	{
		WaveManagerHUD->UpdateCountdown(--CountdownSeconds);
		WaveManagerHUD->PlayCountdownAnim();
	}
	if (CountdownSeconds >= 0) PlaySound(ESound::WaveCountdown);
	else
	{
		StartWave();
		UKismetSystemLibrary::K2_ClearTimer(this, "UpdateCountdown");
		UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate, true);
	}
}
