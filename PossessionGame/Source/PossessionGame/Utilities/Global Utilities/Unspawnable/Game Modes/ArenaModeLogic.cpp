#include "ArenaModeLogic.h"
#include "PossessionGame/Data Assets/Game Modes/ArenaModeData.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UArenaModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	Data = Cast<UArenaModeData>(InGameModeData);
	SetData();
	WaveManager->GetDelegateManager()->OnUpgradeSelectionFinished.BindUObject(this, &UArenaModeLogic::InitiateNextWave);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UArenaModeLogic::UpdateEnemiesAmount);
	SetupWaveManagerUI();
	SetupTimers();
}

void UArenaModeLogic::SetData()
{
	EnemiesPerWave = Data->GetEnemiesPerWave();
	EnemiesLeftToKill = EnemiesPerWave;
	CountdownSeconds = Data->GetCountdownSeconds();
}

void UArenaModeLogic::InitiateNextWave()
{
	bWaveNotStarted = false;
	SetupNextWave();
	if (WaveManagerHUD) StartCountdown();
}

void UArenaModeLogic::SetupNextWave()
{
	EnemiesPerWave += Data->GetEnemiesAmountIncreasePerWave();
	EnemiesLeftToKill = EnemiesPerWave;
	CountdownSeconds = Data->GetCountdownSeconds();
	UpdateCountdown();
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
	GetWorld()->GetTimerManager().SetTimer(CountdownHandle, this, &UArenaModeLogic::UpdateCountdown, 1.f, true);
}

void UArenaModeLogic::UpdateCountdown()
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
		GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UArenaModeLogic::HandleSpawning, SpawnRate, true);
	}
}

void UArenaModeLogic::StartWave()
{
	bInstanceFinished = false;
	PlaySound(ESound::WaveStart);
	PlaySound(ESound::TaskAnnouncement);
	if (!WaveManagerHUD) return;
	WaveManagerHUD->ToggleCountdownText();
}

void UArenaModeLogic::StartCountdown() const { if (WaveManagerHUD) WaveManagerHUD->ToggleCountdownText(); }

void UArenaModeLogic::SetupTimers()
{
	UpdateCountdown();
	GetWorld()->GetTimerManager().SetTimer(CountdownHandle, this, &UArenaModeLogic::UpdateCountdown, 1.f, true);
}

void UArenaModeLogic::SetupWaveManagerUI()
{
	Super::SetupWaveManagerUI();
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateCountdown(CountdownSeconds);
}

void UArenaModeLogic::HandleSpawning()
{
	if (bAsyncLoadInProcess) return;
	if (EnemiesLeftToKill > 0 && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		bAsyncLoadInProcess = true;
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(), [this] { AfterSpawnEnemyCallback(); }, [this] { return bInstanceFinished; });
	}
	else if (!bWaveNotStarted && EnemiesLeftToKill <= 0 && AliveEnemiesAmount <= 0) WaveCompleted();
}

void UArenaModeLogic::WaveCompleted()
{
	if (!WaveManagerHUD) return;
	bInstanceFinished = true;
	WaveManagerHUD->UpdateObjective(EGameMode::ArenaMode, ++CurrentWave);
	WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "WaveCompleted", "Wave Completed"));
	PlaySound(ESound::WaveCompleted);
	bWaveNotStarted = true;
	GetWorld()->GetTimerManager().SetTimer(OpenUpgradeSelectionHandle, this, &UArenaModeLogic::ShowUpgradeSelection, WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
}

void UArenaModeLogic::AfterSpawnEnemyCallback()
{
	bAsyncLoadInProcess = false;
	AliveEnemiesAmount++;
	EnemiesLeftToKill--;
}

void UArenaModeLogic::FadeInObjectivePanel() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjective(EGameMode::ArenaMode, 1);
	Super::FadeInObjectivePanel();
}
