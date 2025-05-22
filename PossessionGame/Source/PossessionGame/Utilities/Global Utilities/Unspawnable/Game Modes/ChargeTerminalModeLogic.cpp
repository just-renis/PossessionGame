#include "ChargeTerminalModeLogic.h"
#include "Kismet/KismetSystemLibrary.h"
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
	UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate * 3.f, true);
}

void UChargeTerminalModeLogic::Finalize()
{
	Super::Finalize();
	if (WaveManagerHUD) WaveManagerHUD->UpdateObjectiveProgress(UsbAmountProgress, Data->GetUsbAmountRequired());
	ShowUpgradeSelection();
	(void)WaveManager->GetDelegateManager()->OnChargeTerminalModeCompletion.ExecuteIfBound();
	ResetTaskProgress();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	WaveManager->GetDelegateManager()->OnUsbInserted.Unbind();
	UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
}

void UChargeTerminalModeLogic::HandleSpawning()
{
	if (!bIsTerminalCharged && AliveEnemiesAmount < EnemiesOnSceneLimit) WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(), [this] { IncreaseAliveEnemiesAmount(); });
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
	}
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjectiveProgress(UsbAmountProgress, Data->GetUsbAmountRequired());
	WaveManagerHUD->UpdateTaskProgress(UsbAmountProgress, UsbAmountRequired);
}

void UChargeTerminalModeLogic::SetupWaveManagerUI()
{
	Super::SetupWaveManagerUI();
	if (!WaveManagerHUD) return;
	FadeOutObjectivePanel();
	if (WaveManagerHUD->GetIsObjectivePanelFadeInAtLeastOnce()) UKismetSystemLibrary::K2_SetTimer(this, "FadeInObjectivePanel", WaveManagerHUD->GetObjectivePanelFadeInEndTime(), false);
	else FadeInObjectivePanel();
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
