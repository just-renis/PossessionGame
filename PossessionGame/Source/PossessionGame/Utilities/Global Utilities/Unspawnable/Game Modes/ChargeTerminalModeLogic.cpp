#include "ChargeTerminalModeLogic.h"
#include "Algo/RandomShuffle.h"
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
	if (Data->GetIsWallLasersExist())
	{
		WaveManager->GetDelegateManager()->OnElevatorFinishedRising.BindUObject(this, &UChargeTerminalModeLogic::InitiateNextCharge);
		ActivateWallLasers();
	}
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
	if (Data->GetIsWallLasersExist()) WaveManager->GetDelegateManager()->OnElevatorFinishedRising.Unbind();
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
	if (!Data->GetIsWallLasersExist()) GetWorld()->GetTimerManager().SetTimer(InitiateNextChargeHandle, this, &UChargeTerminalModeLogic::InitiateNextCharge, WaveManagerHUD->GetWaveAnnouncementEndTime());
	else
	{
		(void)WaveManager->GetDelegateManager()->OnElevatorTriggered.ExecuteIfBound();
		UE_LOG(LogTemp, Error, TEXT("ON ELEVATOR TRIGGERED"));
	}
}

void UChargeTerminalModeLogic::InitiateNextCharge()
{
	if (Data->GetIsWallLasersExist()) ActivateWallLasers();
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

void UChargeTerminalModeLogic::ActivateWallLasers()
{
	TArray Indices = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	Algo::RandomShuffle(Indices);
	Indices.SetNum(LasersAmount++);
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->TriggerWallLasers.Broadcast(Indices, ChargeAmount);
}
