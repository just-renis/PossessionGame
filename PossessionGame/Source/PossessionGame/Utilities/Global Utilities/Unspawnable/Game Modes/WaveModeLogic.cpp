#include "WaveModeLogic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UWaveModeLogic::Init(const TObjectPtr<UWaveManager> InWaveManager)
{
	Super::Init(InWaveManager);
	CurrentWave = 1;
	WaveManager->GetDelegateManager()->OnPlayerDeath.AddUObject(this, &UWaveModeLogic::FadeOutWavePanel);
	WaveManager->GetDelegateManager()->OnUpgradeSelectionFinished.BindUObject(this, &UWaveModeLogic::InitiateNextWave);
	WaveManager->GetDelegateManager()->OnEnemyDeath.AddUObject(this, &UWaveModeLogic::UpdateEnemiesAmount);
	SetupWaveManagerUI();
	SetupTimers();
}

void UWaveModeLogic::Finalize()
{
	WaveManager->GetDelegateManager()->OnPlayerDeath.RemoveAll(this);
	WaveManager->GetDelegateManager()->OnUpgradeSelectionFinished.Unbind();
	WaveManager->GetDelegateManager()->OnEnemyDeath.RemoveAll(this);
	FadeOutWavePanel();
	Super::Finalize();
}

void UWaveModeLogic::SetupWaveManagerUI()
{
	Super::SetupWaveManagerUI();
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateCountdown(CountdownSeconds);
	WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
	WaveManagerHUD->UpdateCurrentWaveText(CurrentWave);
}

void UWaveModeLogic::SetupTimers()
{
	UpdateCountdown();
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateCountdown", 1.f, true);
}

void UWaveModeLogic::HandleSpawning()
{
	if (EnemiesLeftToKill > 0 && AliveEnemiesAmount < EnemiesOnSceneLimit) WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, [this] { WaveModeCallback(); });
	else if (!bWaveNotStarted && EnemiesLeftToKill <= 0 && AliveEnemiesAmount <= 0) WaveCompleted();
}

void UWaveModeLogic::UpdateEnemiesAmount(const int32 InXp)
{
	Super::UpdateEnemiesAmount(InXp);
	if (WaveManagerHUD) WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
}

void UWaveModeLogic::InitiateNextWave()
{
	if (CurrentWave++ < AmountOfWaves)
	{
		bWaveNotStarted = false;
		SetupNextWave();
		if (WaveManagerHUD)
		{
			SetupNextWaveUI();
			StartCountdown();
		}
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
	WaveManagerHUD->ToggleEnemiesLeftText();
	WaveManagerHUD->ToggleCurrentWaveText();
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

void UWaveModeLogic::SetupNextWaveUI() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->ToggleCurrentWaveText();
	WaveManagerHUD->ToggleEnemiesLeftText();
	WaveManagerHUD->UpdateCurrentWaveText(CurrentWave);
	WaveManagerHUD->UpdateEnemiesAmount(EnemiesLeftToKill + AliveEnemiesAmount);
}

void UWaveModeLogic::FadeOutWavePanel() const { if (WaveManagerHUD) WaveManagerHUD->PlayWavePanelFadeOutAnim(); }

void UWaveModeLogic::WaveCompleted()
{
	if (!WaveManagerHUD) return;
	PlaySound(ESound::WaveCompleted);
	bWaveNotStarted = true;
	WaveManagerHUD->PlayAnnouncementAnim("Wave Completed");
	UKismetSystemLibrary::K2_SetTimer(this, "ShowUpgradeSelection", WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
}

void UWaveModeLogic::WaveModeCallback()
{
	UE_LOG(LogTemp, Error, TEXT("WAVE MODE CALLBACK"));
	AliveEnemiesAmount++;
	EnemiesLeftToKill--;
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
