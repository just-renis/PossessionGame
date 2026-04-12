#include "MultipleWardenModeLogic.h"
#include "PossessionGame/Data Assets/Game Modes/MultipleWardenModeData.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UMultipleWardenModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	Data = Cast<UMultipleWardenModeData>(InGameModeData);
	SetupWaveManagerUI();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UMultipleWardenModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->OnUsbInserted.AddUObject(this, &UMultipleWardenModeLogic::UpdateTaskProgress);
	PlaySound(ESound::TaskAnnouncement);
	SpawnMiniBosses();
}

void UMultipleWardenModeLogic::Finalize()
{
	Super::Finalize();
	if (WaveManagerHUD)
	{
		WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "BarriersDisabled", "Barriers Disabled"));
		WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	}
	WaveManager->GetDelegateManager()->StartForceFieldDissolve.Broadcast(0);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	WaveManager->GetDelegateManager()->OnUsbInserted.RemoveAll(this);
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
}

void UMultipleWardenModeLogic::HandleSpawning()
{
	Super::HandleSpawning();
	if (bAsyncLoadInProcess) return;
	if (!bIsTerminalsCharged && !bIsUsbRequirementFilled && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(),
			[this](const TObjectPtr<AEnemy>&) { IncreaseAliveEnemiesAmount(); }, [this] { return bInstanceFinished; });
	}
}

void UMultipleWardenModeLogic::UpdateEnemiesAmount()
{
	Super::UpdateEnemiesAmount();
	if (AliveEnemiesAmount > 0) return;
	if (bIsTerminalsCharged) Finalize();
	else if (bIsUsbRequirementFilled) ChargesCleared();
}

void UMultipleWardenModeLogic::UpdateTaskProgress()
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjectiveProgress(ChargeAmount, Data->GetChargeAmountRequired());
	if (++ChargeAmount >= Data->GetChargeAmountRequired())
	{
		bIsTerminalsCharged = true;
		if (AliveEnemiesAmount <= 0) Finalize();
		else WaveManagerHUD->ToggleFinishOffEnemiesText(true);
	}
	else
	{
		if (ChargeAmount % Data->GetBossesAtOnceSpawned() == 0) UsbRequirementFilled();
		
	}
}

void UMultipleWardenModeLogic::FadeInObjectivePanel() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjective(EGameMode::MultipleWardenMode, Data->GetChargeAmountRequired());
	WaveManagerHUD->UpdateObjectiveProgress(0, Data->GetChargeAmountRequired());
	Super::FadeInObjectivePanel();
}

void UMultipleWardenModeLogic::UsbRequirementFilled()
{
	if (AliveEnemiesAmount <= 0) ChargesCleared();
	else
	{
		bIsUsbRequirementFilled = true;
		WaveManagerHUD->ToggleFinishOffEnemiesText(true);
	}
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
}

void UMultipleWardenModeLogic::ChargesCleared()
{
	if (!WaveManagerHUD) return;
	bInstanceFinished = true;
	TotalAmountOfEnemiesKilled = 0;
	WaveManagerHUD->PlayAnnouncementAnim(NSLOCTEXT("ST_Localization", "WaveCompleted", "Wave Completed"));
	FTimerHandle InitiateNextChargeHandle;
	GetWorld()->GetTimerManager().SetTimer(InitiateNextChargeHandle, this, &UMultipleWardenModeLogic::InitiateNextCharge, WaveManagerHUD->GetWaveAnnouncementEndTime());
}

void UMultipleWardenModeLogic::InitiateNextCharge()
{
	bIsUsbRequirementFilled = false;
	bInstanceFinished = false;
	WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	SpawnMiniBosses();
}

void UMultipleWardenModeLogic::SpawnMiniBosses()
{
	(void)WaveManager->GetDelegateManager()->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Warden, Data->GetBossesAtOnceSpawned());
	if (Data->GetBossesAtOnceSpawned() > 0) SpawnMiniBoss(EGameMode::MiniBossSpawn1);
	if (Data->GetBossesAtOnceSpawned() > 1) SpawnMiniBoss(EGameMode::MiniBossSpawn2);
}

void UMultipleWardenModeLogic::SpawnMiniBoss(const EGameMode InGameModeSpawn)
{
	WaveManager->GetAssetsManager()->RequestSpawnEnemy(Data->GetEnemyToSpawn(), InGameModeSpawn,
	[this](const TObjectPtr<AEnemy>&)
	{
		IncreaseAliveEnemiesAmount();
		GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UMultipleWardenModeLogic::HandleSpawning, SpawnRate, true);
	}, nullptr);
}
