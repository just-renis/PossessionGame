#include "WaveModeLogic.h"
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
}

void UWaveModeLogic::SetupTimers()
{
	UpdateCountdown();
	GetWorld()->GetTimerManager().SetTimer(CountdownHandle, this, &UWaveModeLogic::UpdateCountdown, 1.f, true);
}

void UWaveModeLogic::HandleSpawning()
{
	if (bAsyncLoadInProcess) return;
	if (EnemiesLeftToKill > 0 && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		bAsyncLoadInProcess = true;
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(), true, [this] { AfterSpawnEnemyCallback(); });
	}
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
	EnemiesPerWave += Data->GetEnemiesAmountIncreasePerWave();
	EnemiesLeftToKill = EnemiesPerWave;
	CountdownSeconds = 4.f;
	UpdateCountdown();
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
	GetWorld()->GetTimerManager().SetTimer(CountdownHandle, this, &UWaveModeLogic::UpdateCountdown, 1.f, true);
}

void UWaveModeLogic::WaveCompleted()
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjectiveProgress(CurrentWave, AmountOfWaves);
	WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "WaveCompleted", "Wave Completed"));
	PlaySound(ESound::WaveCompleted);
	bWaveNotStarted = true;
	GetWorld()->GetTimerManager().SetTimer(OpenUpgradeSelectionHandle, this, &UWaveModeLogic::ShowUpgradeSelection, WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
}

void UWaveModeLogic::AfterSpawnEnemyCallback()
{
	bAsyncLoadInProcess = false;
	AliveEnemiesAmount++;
	EnemiesLeftToKill--;
}

void UWaveModeLogic::SetData()
{
	EnemiesPerWave = Data->GetEnemiesPerWave();
	EnemiesLeftToKill = EnemiesPerWave;
	AmountOfWaves = Data->GetAmountOfWaves();
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
		GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
		GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UWaveModeLogic::HandleSpawning, SpawnRate, true);
	}
}
