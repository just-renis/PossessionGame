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
	SetupWaveManagerUI();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UChargeTerminalModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->OnUsbInserted.AddUObject(this, &UChargeTerminalModeLogic::UpdateTaskProgress);
	WaveManager->GetDelegateManager()->OnUpgradeSelectionFinished.BindUObject(this, &UChargeTerminalModeLogic::InitiateNextCharge);
	PlaySound(ESound::TaskAnnouncement);
	WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Warden, EGameMode::MiniBossSpawn, [this](const float)
	{
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
		WaveManagerHUD->UpdateObjectiveProgress(ChargeAmount, Data->GetChargeAmountRequired(), true);
		WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	}
	WaveManager->GetDelegateManager()->StartForceFieldDissolve.Broadcast(0);
	WaveManager->GetDelegateManager()->OnUpgradeSelectionFinished.Unbind();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	WaveManager->GetDelegateManager()->OnUsbInserted.RemoveAll(this);
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
}

void UChargeTerminalModeLogic::HandleSpawning()
{
	if (bAsyncLoadInProcess) return;
	if (!bIsTerminalCharged && !bIsUsbRequirementFilled && AliveEnemiesAmount < EnemiesOnSceneLimit) WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(), [this](const float) { IncreaseAliveEnemiesAmount(); }, [this] { return bInstanceFinished; });
}

void UChargeTerminalModeLogic::UpdateEnemiesAmount()
{
	Super::UpdateEnemiesAmount();
	if (AliveEnemiesAmount > 0) return;
	if (bIsTerminalCharged) Finalize();
	else if (bIsUsbRequirementFilled) ChargeCleared();
}

void UChargeTerminalModeLogic::UpdateTaskProgress()
{
	if (!WaveManagerHUD) return;
	if (++ChargeAmount >= Data->GetChargeAmountRequired())
	{
		bIsTerminalCharged = true;
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
	WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "WaveCompleted", "Wave Completed"));
	GetWorld()->GetTimerManager().SetTimer(NextChargeHandle, this, &UChargeTerminalModeLogic::ShowUpgradeSelection, WaveManagerHUD->GetWaveAnnouncementEndTime(), false);
}

void UChargeTerminalModeLogic::InitiateNextCharge()
{
	bIsUsbRequirementFilled = false;
	bInstanceFinished = false;
	WaveManagerHUD->UpdateObjectiveProgress(ChargeAmount, Data->GetChargeAmountRequired(), true);
	WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Warden, EGameMode::MiniBossSpawn, [this](const float) { IncreaseAliveEnemiesAmount(); }, nullptr);
	GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UChargeTerminalModeLogic::HandleSpawning, SpawnRate, true);
}
