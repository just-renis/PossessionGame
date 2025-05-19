#include "BossModeLogic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"

void UBossModeLogic::Init(const TObjectPtr<UWaveManager> InWaveManager)
{
	Super::Init(InWaveManager);
	EnemiesOnSceneLimit = 4;
	WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::MinionBoss, [this] { IncreaseAliveEnemiesAmount(); });
	WaveManager->GetDelegateManager()->OnEnemyDeath.AddUObject(this, &UBossModeLogic::UpdateEnemiesAmount);
	WaveManager->GetDelegateManager()->OnBossDeath.BindUObject(this, &UBossModeLogic::BossDeath);
	UKismetSystemLibrary::K2_SetTimer(this, "HandleSpawning", SpawnRate * 3.f, true);
}

void UBossModeLogic::Finalize()
{
	Super::Finalize();
	ShowUpgradeSelection();
	WaveManager->GetDelegateManager()->OnEnemyDeath.RemoveAll(this);
	WaveManager->GetDelegateManager()->OnBossDeath.Unbind();
}

void UBossModeLogic::HandleSpawning()
{
	if (bIsBossAlive && AliveEnemiesAmount < EnemiesOnSceneLimit)
	{
		WaveManager->GetAssetsManager()->RequestSpawnEnemy(ECharacterType::Random, [this] { IncreaseAliveEnemiesAmount(); });
	}
}

void UBossModeLogic::UpdateEnemiesAmount(const int32 InXp)
{
	Super::UpdateEnemiesAmount(InXp);
	if (!bIsBossAlive && AliveEnemiesAmount <= 0) Finalize();
}

void UBossModeLogic::BossDeath()
{
	bIsBossAlive = false;
	UKismetSystemLibrary::K2_ClearTimer(this, "HandleSpawning");
}
