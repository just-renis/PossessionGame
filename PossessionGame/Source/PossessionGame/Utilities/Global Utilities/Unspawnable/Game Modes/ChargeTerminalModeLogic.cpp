#include "ChargeTerminalModeLogic.h"
#include "PossessionGame/Data Assets/Game Modes/ChargeTerminalModeData.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UChargeTerminalModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	Data = Cast<UChargeTerminalModeData>(InGameModeData);
	SetupWaveManagerUI();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UChargeTerminalModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->OnUsbInserted.AddUObject(this, &UChargeTerminalModeLogic::UpdateTaskProgress);
	PlaySound(ESound::TaskAnnouncement);
	WaveManager->GetAssetsManager()->RequestSpawnEnemy(Data->GetEnemyToSpawn(), EGameMode::MiniBossSpawn,
	[this](const TObjectPtr<AEnemy>&)
	{
		(void)WaveManager->GetDelegateManager()->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Warden, 1);
		IncreaseAliveEnemiesAmount();
		GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UChargeTerminalModeLogic::HandleSpawning, SpawnRate, true);
	}, nullptr);
}

void UChargeTerminalModeLogic::Finalize()
{
	Super::Finalize();
	if (WaveManagerHUD)
	{
		WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "BarrierDisabled", "Barrier Disabled"));
		WaveManagerHUD->UpdateObjectiveProgress(ChargeAmount, Data->GetChargeAmountRequired());
		WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	}
	WaveManager->GetDelegateManager()->StartForceFieldDissolve.Broadcast(0);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	WaveManager->GetDelegateManager()->OnUsbInserted.RemoveAll(this);
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
}

void UChargeTerminalModeLogic::HandleSpawning()
{
	if (bAsyncLoadInProcess) return;
	if (!bIsTerminalsCharged && !bIsUsbRequirementFilled && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(),
			[this](const TObjectPtr<AEnemy>&) { IncreaseAliveEnemiesAmount(); }, [this] { return bInstanceFinished; });
	}
}

void UChargeTerminalModeLogic::UpdateEnemiesAmount()
{
	Super::UpdateEnemiesAmount();
	if (AliveEnemiesAmount > 0) return;
	if (bIsTerminalsCharged) Finalize();
	else if (bIsUsbRequirementFilled) ChargeCleared();
}

void UChargeTerminalModeLogic::UpdateTaskProgress()
{
	if (!WaveManagerHUD) return;
	if (++ChargeAmount >= Data->GetChargeAmountRequired())
	{
		bIsTerminalsCharged = true;
		if (AliveEnemiesAmount <= 0) Finalize();
		else WaveManagerHUD->ToggleFinishOffEnemiesText(true);
	}
	else UsbRequirementFilled();
}

void UChargeTerminalModeLogic::FadeInObjectivePanel() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjective(EGameMode::ChargeTerminalMode, Data->GetChargeAmountRequired());
	WaveManagerHUD->UpdateObjectiveProgress(0.f, Data->GetChargeAmountRequired());
	Super::FadeInObjectivePanel();
}

void UChargeTerminalModeLogic::UsbRequirementFilled()
{
	if (AliveEnemiesAmount <= 0) ChargeCleared();
	else
	{
		bIsUsbRequirementFilled = true;
		WaveManagerHUD->ToggleFinishOffEnemiesText(true);
	}
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
}

void UChargeTerminalModeLogic::ChargeCleared()
{
	if (!WaveManagerHUD) return;
	bInstanceFinished = true;
	TotalAmountOfEnemiesKilled = 0;
	WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "WaveCompleted", "Wave Completed"));
	FTimerHandle InitiateNextChargeHandle;
	GetWorld()->GetTimerManager().SetTimer(InitiateNextChargeHandle, this, &UChargeTerminalModeLogic::InitiateNextCharge, WaveManagerHUD->GetWaveAnnouncementEndTime());
}

void UChargeTerminalModeLogic::InitiateNextCharge()
{
	bIsUsbRequirementFilled = false;
	bInstanceFinished = false;
	WaveManagerHUD->UpdateObjectiveProgress(ChargeAmount, Data->GetChargeAmountRequired());
	WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	WaveManager->GetAssetsManager()->RequestSpawnEnemy(Data->GetEnemyToSpawn(), EGameMode::MiniBossSpawn,
	[this](const TObjectPtr<AEnemy>&)
	{
		(void)WaveManager->GetDelegateManager()->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Warden, 1);
		IncreaseAliveEnemiesAmount();
		GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UChargeTerminalModeLogic::HandleSpawning, SpawnRate, true);
	}, nullptr);
}
