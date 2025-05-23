#include "BossModeLogic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UBossModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	SetupWaveManagerUI();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UBossModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->OnBossDeath.BindUObject(this, &UBossModeLogic::BossDeath);
	WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::MinionBoss, WaveManager->GetCurrentGameMode(), [this] { IncreaseAliveEnemiesAmount(); });
	UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate, true);
}

void UBossModeLogic::Finalize()
{
	Super::Finalize();
	UE_LOG(LogTemp, Error, TEXT("IS BOSS DEAD? %i"), !bIsBossAlive);
	WaveManagerHUD->UpdateObjectiveProgress(!bIsBossAlive, 1);
	ShowUpgradeSelection();
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	WaveManager->GetDelegateManager()->OnBossDeath.Unbind();
}

void UBossModeLogic::HandleSpawning()
{
	if (bIsBossAlive && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, WaveManager->GetCurrentGameMode(), [this] { IncreaseAliveEnemiesAmount(); });
	}
}

void UBossModeLogic::UpdateEnemiesAmount()
{
	Super::UpdateEnemiesAmount();
	if (!bIsBossAlive && AliveEnemiesAmount <= 0) Finalize();
}

void UBossModeLogic::SetupWaveManagerUI()
{
	Super::SetupWaveManagerUI();
	if (!WaveManagerHUD) return;
	FadeOutObjectivePanel();
	if (WaveManagerHUD->GetIsObjectivePanelFadeInAtLeastOnce()) UKismetSystemLibrary::K2_SetTimer(this, "FadeInObjectivePanel", WaveManagerHUD->GetObjectivePanelFadeInEndTime(), false);
	else FadeInObjectivePanel();
}

void UBossModeLogic::FadeInObjectivePanel() const
{
	if (!WaveManagerHUD) return;
	WaveManagerHUD->UpdateObjective(EGameMode::BossMode, 1);
	WaveManagerHUD->UpdateObjectiveProgress(!bIsBossAlive, 1);
	Super::FadeInObjectivePanel();
}

void UBossModeLogic::BossDeath()
{
	bIsBossAlive = false;
	UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
}
