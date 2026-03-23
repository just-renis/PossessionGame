#include "BossModeLogic.h"
#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "PossessionGame/Data Assets/Game Modes/BossModeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UBossModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	Data = Cast<UBossModeData>(InGameModeData);
	SetupWaveManagerUI();
	WaveManager->GetDelegateManager()->OnBossDeath.AddUObject(this, &UBossModeLogic::BossDeath);
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UBossModeLogic::UpdateEnemiesAmount);
	if (!Data->GetIsFinalBoss()) WaveManager->GetAssetsManager()->RequestSpawnEnemy(Data->GetEnemyToSpawn(), WaveManager->GetCurrentGameMode(), [this](const float) { IncreaseAliveEnemiesAmount(); }, nullptr);
	else (void)WaveManager->GetDelegateManager()->OnFinalBossModeInitiated.ExecuteIfBound();
	GetWorld()->GetTimerManager().SetTimer(SpawnHandle, this, &UBossModeLogic::HandleSpawning, SpawnRate, true);
}

void UBossModeLogic::Finalize()
{
	Super::Finalize();
	TotalAmountOfEnemiesKilled = 0;
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.Unbind();
	WaveManager->GetDelegateManager()->OnBossDeath.RemoveAll(this);
	if (!WaveManagerHUD) return;
	WaveManagerHUD->ToggleFinishOffEnemiesText(false);
	WaveManagerHUD->UpdateObjectiveProgress(!bIsBossAlive, 1);
}

void UBossModeLogic::HandleSpawning()
{
	if (bAsyncLoadInProcess) return;
 	if (bIsBossAlive && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, EGameMode::BossModeEnemy, [this](const float) { IncreaseAliveEnemiesAmount(); }, [this] { return !bIsBossAlive; });
	}
}

void UBossModeLogic::UpdateEnemiesAmount()
{
	Super::UpdateEnemiesAmount();
	if (!bIsBossAlive && AliveEnemiesAmount <= 0) Finalize();
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
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
	if (AliveEnemiesAmount <= 0) Finalize();
	else if (WaveManagerHUD) WaveManagerHUD->ToggleFinishOffEnemiesText(true);
}
