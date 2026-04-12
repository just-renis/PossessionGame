#include "HackTerminalModeLogic.h"

#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Game Modes/HackTerminalModeData.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UHackTerminalModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	Data = Cast<UHackTerminalModeData>(InGameModeData);
	UpdateHpPoolInitialValues();
	SetupWaveManagerUI();
	PlaySound(ESound::GeneratorDisableAnnouncement);
	WaveManager->GetDelegateManager()->OnHackModeStart.Broadcast();
	WaveManager->GetDelegateManager()->OnTerminalInteraction.AddUObject(this, &UHackTerminalModeLogic::StartTerminalHack);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UHackTerminalModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->SendHackTerminalRoomMode.AddUObject(this, &UHackTerminalModeLogic::SetTerminalRoomMode);
	WaveManager->GetDelegateManager()->HackTerminalSubtractHp.BindUObject(this, &UHackTerminalModeLogic::UpdateHpPoolBar);
}

void UHackTerminalModeLogic::Finalize()
{
	WaveManager->GetDelegateManager()->OnTerminalInteraction.RemoveAll(this);
	WaveManager->GetDelegateManager()->SendHackTerminalRoomMode.RemoveAll(this);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	WaveManager->GetDelegateManager()->HackTerminalSubtractHp.Unbind();
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
	Super::Finalize();
}

void UHackTerminalModeLogic::HandleSpawning()
{
	if (bAsyncLoadInProcess) return;
	if (!bInstanceFinished && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		bAsyncLoadInProcess = true;
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, CurrentTerminalRoomMode,
		[this](const TObjectPtr<AEnemy>& OutEnemy)
		{
			IncreaseAliveEnemiesAmount();
			ReservedEnemiesHpPool -= OutEnemy->GetCurrentHp();
			if (ReservedEnemiesHpPool <= 0.f)
			{
				UnreservedEnemiesHpPool -= ReservedEnemiesHpPool;
				StopSpawning();
			}
		}, [this] { return bInstanceFinished; });
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
	(void)WaveManager->GetDelegateManager()->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::HackTerminalBar, 1);
	if (WaveManagerHUD) WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "HackInitiated", "Hack Initiated"));
	GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UHackTerminalModeLogic::HandleSpawning, SpawnRate, true);
}

void UHackTerminalModeLogic::FinishTerminalHack()
{
	PlaySound(ESound::GeneratorCompleted);
	TotalAmountOfEnemiesKilled = 0;
	UpdateHpPoolInitialValues();
	WaveManager->GetDelegateManager()->OnBossDeath.Broadcast();
	WaveManager->GetDelegateManager()->OnTerminalHackRoomFinish.Broadcast(CurrentTerminalRoomMode);
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
}

void UHackTerminalModeLogic::StopSpawning()
{
	bInstanceFinished = true;
	ReservedEnemiesHpPool = 0.f;
	if (AliveEnemiesAmount <= 0) FinishTerminalHack();
	else if (WaveManagerHUD) WaveManagerHUD->ToggleFinishOffEnemiesText(true);
}

void UHackTerminalModeLogic::UpdateHpPoolBar(const float InHp)
{
	UnreservedEnemiesHpPool -= InHp;
	(void)WaveManager->GetDelegateManager()->UpdateGlobalHpBar.ExecuteIfBound(UnreservedEnemiesHpPool, TotalEnemiesHpPool);
}

void UHackTerminalModeLogic::UpdateHpPoolInitialValues()
{
	TotalEnemiesHpPool = ReservedEnemiesHpPool = UnreservedEnemiesHpPool = Data->GetEnemiesHpPool();
	if (WaveManager->GetDelegateManager()->RequestPlayerHp.IsBound())
	{
		UnreservedEnemiesHpPool += WaveManager->GetDelegateManager()->RequestPlayerHp.Execute();
		TotalEnemiesHpPool = UnreservedEnemiesHpPool;
	}
}
