#include "HackTerminalModeLogic.h"
#include "PossessionGame/Data Assets/Game Modes/HackTerminalModeData.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UHackTerminalModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	Data = Cast<UHackTerminalModeData>(InGameModeData);
	SetData();
	SetupWaveManagerUI();
	PlaySound(ESound::GeneratorDisableAnnouncement);
	WaveManager->GetDelegateManager()->OnHackModeStart.Broadcast();
	WaveManager->GetDelegateManager()->OnTerminalInteraction.AddUObject(this, &UHackTerminalModeLogic::StartTerminalHack);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UHackTerminalModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->SendHackTerminalRoomMode.AddUObject(this, &UHackTerminalModeLogic::SetTerminalRoomMode);
}

void UHackTerminalModeLogic::Finalize()
{
	ResetTaskProgress();
	WaveManager->GetDelegateManager()->OnTerminalInteraction.RemoveAll(this);
	WaveManager->GetDelegateManager()->SendHackTerminalRoomMode.RemoveAll(this);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
	Super::Finalize();
}

void UHackTerminalModeLogic::HandleSpawning()
{
	if (bAsyncLoadInProcess) return;
	if (!bInstanceFinished && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		bAsyncLoadInProcess = true;
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, CurrentTerminalRoomMode, [this] { IncreaseAliveEnemiesAmount(); }, [this] { return bInstanceFinished; });
	}
}

void UHackTerminalModeLogic::UpdateTaskProgress()
{
	TerminalHackProgress += 0.5f;
	if (WaveManagerHUD) WaveManagerHUD->UpdateTaskProgress(TerminalHackProgress, TerminalHackRequired);
	if (TerminalHackProgress >= TerminalHackRequired)
	{
		bInstanceFinished = true;
		if (AliveEnemiesAmount <= 0) FinishTerminalHack();
		else if (WaveManagerHUD) WaveManagerHUD->ToggleFinishOffEnemiesText(true);
		GetWorld()->GetTimerManager().ClearTimer(TaskProgressHandle);
	}
}

void UHackTerminalModeLogic::UpdateEnemiesAmount()
{
	Super::UpdateEnemiesAmount();
	if (bInstanceFinished && AliveEnemiesAmount <= 0) FinishTerminalHack();
}

void UHackTerminalModeLogic::FadeInObjectivePanel() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjective(EGameMode::HackTerminalsMode, Data->GetTerminalsAmountToHack());
	WaveManagerHUD->UpdateObjectiveProgress(0, Data->GetTerminalsAmountToHack());
	Super::FadeInObjectivePanel();
}

void UHackTerminalModeLogic::StartTerminalHack()
{
	bInstanceFinished = false;
	PlaySound(ESound::GeneratorInitiate);
	PlaySound(ESound::TaskAnnouncement);
	if (WaveManagerHUD)
	{
		WaveManagerHUD->ToggleTaskProgress(true, WaveManager->GetCurrentGameMode());
		WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "HackInitiated", "Hack Initiated"));
	}
	GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UHackTerminalModeLogic::HandleSpawning, SpawnRate, true);
	GetWorld()->GetTimerManager().SetTimer(TaskProgressHandle, this, &UHackTerminalModeLogic::UpdateTaskProgress, 0.5f, true);
}

void UHackTerminalModeLogic::FinishTerminalHack()
{
	PlaySound(ESound::GeneratorCompleted);
	WaveManager->GetDelegateManager()->OnTerminalHackRoomFinish.Broadcast(CurrentTerminalRoomMode);
	TerminalHackProgress = 0.f;
	if (++TerminalsHackedAmount >= Data->GetTerminalsAmountToHack()) Finalize();
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
		WaveManager->GetDelegateManager()->OnTerminalHackFinish.Broadcast(WaveManager->GetCurrentGameMode());
	}
	if (!WaveManagerHUD) return;
	WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "HackCompleted", "Hack Completed"));
	WaveManagerHUD->UpdateObjectiveProgress(TerminalsHackedAmount, Data->GetTerminalsAmountToHack());
	GetWorld()->GetTimerManager().SetTimer(OpenUpgradeSelectionHandle, this, &UHackTerminalModeLogic::ShowUpgradeSelection, WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
}

void UHackTerminalModeLogic::SetData() { TerminalHackRequired = Data->GetTerminalHackRequired(); }
