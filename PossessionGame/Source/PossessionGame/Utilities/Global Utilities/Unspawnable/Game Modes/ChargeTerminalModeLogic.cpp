#include "ChargeTerminalModeLogic.h"
#include "PossessionGame/Data Assets/Game Modes/ChargeTerminalModeData.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UChargeTerminalModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	Data = Cast<UChargeTerminalModeData>(InGameModeData);
	SetData();
	SetupWaveManagerUI();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UChargeTerminalModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->OnUsbInserted.BindUObject(this, &UChargeTerminalModeLogic::UpdateTaskProgress);
	PlaySound(ESound::TaskAnnouncement);
	if (WaveManagerHUD) WaveManagerHUD->ToggleTaskProgress(true, WaveManager->GetCurrentGameMode());
	GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UChargeTerminalModeLogic::HandleSpawning, SpawnRate, true);
}

void UChargeTerminalModeLogic::Finalize()
{
	Super::Finalize();
	if (WaveManagerHUD)
	{
		WaveManagerHUD->UpdateObjectiveProgress(UsbAmountProgress, Data->GetUsbAmountRequired(), true);
		WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	}
	WaveManager->GetDelegateManager()->StartForceFieldDissolve.Broadcast(0);
	ResetTaskProgress();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	WaveManager->GetDelegateManager()->OnUsbInserted.Unbind();
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
}

void UChargeTerminalModeLogic::HandleSpawning()
{
	if (!bIsTerminalCharged && AliveEnemiesAmount < EnemiesOnSceneLimit) WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(), true, [this] { IncreaseAliveEnemiesAmount(); });
}

void UChargeTerminalModeLogic::UpdateEnemiesAmount()
{
	Super::UpdateEnemiesAmount();
	if (bIsTerminalCharged && AliveEnemiesAmount <= 0) Finalize();
}

void UChargeTerminalModeLogic::UpdateTaskProgress()
{
	if (++UsbAmountProgress >= UsbAmountRequired)
	{
		bIsTerminalCharged = true;
		if (AliveEnemiesAmount <= 0) Finalize();
		else if (WaveManagerHUD) WaveManagerHUD->ToggleFinishOffEnemiesText(true);
	}
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjectiveProgress(UsbAmountProgress, Data->GetUsbAmountRequired());
	WaveManagerHUD->UpdateTaskProgress(UsbAmountProgress, UsbAmountRequired);
}

void UChargeTerminalModeLogic::FadeInObjectivePanel() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjective(EGameMode::ChargeTerminalMode, Data->GetUsbAmountRequired());
	WaveManagerHUD->UpdateObjectiveProgress(0, Data->GetUsbAmountRequired());
	Super::FadeInObjectivePanel();
}

void UChargeTerminalModeLogic::SetData()
{
	UsbAmountRequired = Data->GetUsbAmountRequired();
}
