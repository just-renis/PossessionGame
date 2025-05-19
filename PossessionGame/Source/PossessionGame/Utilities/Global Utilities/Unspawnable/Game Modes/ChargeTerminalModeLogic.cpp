#include "ChargeTerminalModeLogic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UChargeTerminalModeLogic::Init(const TObjectPtr<UWaveManager> InWaveManager)
{
	Super::Init(InWaveManager);
	SetupWaveManagerUI();
	WaveManager->GetDelegateManager()->OnEnemyDeath.AddUObject(this, &UChargeTerminalModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->OnUsbInserted.BindUObject(this, &UChargeTerminalModeLogic::UpdateTaskProgress);
	PlaySound(ESound::TaskAnnouncement);
	if (WaveManagerHUD) WaveManagerHUD->ToggleTaskProgress(true, WaveManager->GetAssetsManager()->GetCurrentGameMode());
	UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate * 3.f, true);
}

void UChargeTerminalModeLogic::Finalize()
{
	Super::Finalize();
	ShowUpgradeSelection();
	(void)WaveManager->GetDelegateManager()->OnChargeTerminalModeCompletion.ExecuteIfBound();
	ResetTaskProgress();
	WaveManager->GetDelegateManager()->OnEnemyDeath.RemoveAll(this);
	WaveManager->GetDelegateManager()->OnUsbInserted.Unbind();
	UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
}

void UChargeTerminalModeLogic::HandleSpawning()
{
	if (!bIsTerminalCharged && AliveEnemiesAmount < EnemiesOnSceneLimit) WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, [this] { IncreaseAliveEnemiesAmount(); });
}

void UChargeTerminalModeLogic::UpdateEnemiesAmount(const int32 InXp)
{
	Super::UpdateEnemiesAmount(InXp);
	if (bIsTerminalCharged && AliveEnemiesAmount <= 0) Finalize();
}

void UChargeTerminalModeLogic::UpdateTaskProgress()
{
	TerminalHackProgress += TerminalHackRequired * 0.25f;
	if (WaveManagerHUD) WaveManagerHUD->UpdateTaskProgress(TerminalHackProgress, TerminalHackRequired);
	if (TerminalHackProgress >= TerminalHackRequired)
	{
		bIsTerminalCharged = true;
		if (AliveEnemiesAmount <= 0) Finalize();
	}
}
