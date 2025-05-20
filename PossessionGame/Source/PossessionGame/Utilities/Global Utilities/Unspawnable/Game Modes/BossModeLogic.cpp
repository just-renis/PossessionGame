#include "BossModeLogic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UBossModeLogic::Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData)
{
	Super::Init(InWaveManager, InGameModeData);
	WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::MinionBoss, WaveManager->GetCurrentGameMode(), [this] { IncreaseAliveEnemiesAmount(); });
	WaveManager->GetDelegateManager()->UpdateGameModeEnemiesAmount.BindUObject(this, &UBossModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->OnBossDeath.BindUObject(this, &UBossModeLogic::BossDeath);
	UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate * 3.f, true);
}

void UBossModeLogic::Finalize()
{
	Super::Finalize();
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

void UBossModeLogic::BossDeath()
{
	bIsBossAlive = false;
	UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
}
