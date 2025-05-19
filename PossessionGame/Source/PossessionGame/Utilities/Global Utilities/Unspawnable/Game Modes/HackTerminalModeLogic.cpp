#include "HackTerminalModeLogic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UHackTerminalModeLogic::Init(const TObjectPtr<UWaveManager> InWaveManager)
{
	Super::Init(InWaveManager);
	SetupWaveManagerUI();
	PlaySound(ESound::GeneratorDisableAnnouncement);
	WaveManager->GetDelegateManager()->OnHackModeStart.Broadcast();
	WaveManager->GetDelegateManager()->OnTerminalInteraction.AddUObject(this, &UHackTerminalModeLogic::StartTerminalHack);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UHackTerminalModeLogic::UpdateEnemiesAmount);
}

void UHackTerminalModeLogic::Finalize()
{
	ResetTaskProgress();
	WaveManager->GetDelegateManager()->OnTerminalInteraction.RemoveAll(this);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
	Super::Finalize();
}

void UHackTerminalModeLogic::HandleSpawning()
{
	if (!bIsHackFinished && AliveEnemiesAmount < EnemiesOnSceneLimit) WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(), [this] { IncreaseAliveEnemiesAmount(); });
}

void UHackTerminalModeLogic::UpdateTaskProgress()
{
	TerminalHackProgress += 0.5f;
	if (WaveManagerHUD) WaveManagerHUD->UpdateTaskProgress(TerminalHackProgress, TerminalHackRequired);
	if (TerminalHackProgress >= TerminalHackRequired)
	{
		bIsHackFinished = true;
		if (AliveEnemiesAmount <= 0) FinishTerminalHack();
		UKismetSystemLibrary::K2_ClearTimer(this, "UpdateTaskProgress");
	}
}

void UHackTerminalModeLogic::UpdateEnemiesAmount()
{
	Super::UpdateEnemiesAmount();
	if (bIsHackFinished && AliveEnemiesAmount <= 0) FinishTerminalHack();
}

void UHackTerminalModeLogic::StartTerminalHack()
{
	bIsHackFinished = false;
	PlaySound(ESound::GeneratorInitiate);
	PlaySound(ESound::TaskAnnouncement);
	if (WaveManagerHUD)
	{
		WaveManagerHUD->ToggleTaskProgress(true, WaveManager->GetCurrentGameMode());
		WaveManagerHUD->PlayAnnouncementAnim("Hack Initiated");
	}
	UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate, true);
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateTaskProgress", 0.5f, true);
}

void UHackTerminalModeLogic::FinishTerminalHack()
{
	if (WaveManagerHUD)
	{
		WaveManagerHUD->PlayAnnouncementAnim("Hack Completed");
		UKismetSystemLibrary::K2_SetTimer(this, "ShowUpgradeSelection", WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
	}
	PlaySound(ESound::GeneratorCompleted);
	TerminalHackProgress = 0.f;
	if (--TerminalsLeftToHack <= 0) Finalize();
	else
	{
		UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
		WaveManager->GetDelegateManager()->OnTerminalHackFinish.Broadcast(WaveManager->GetCurrentGameMode());
	}
}
