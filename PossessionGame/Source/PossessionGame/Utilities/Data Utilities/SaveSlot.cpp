#include "SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

int32 USaveSlot::GetLevelOfAnUpgrade(const EUpgrade InUpgrade) { return UnlockedUpgrades.Find(InUpgrade) ? UnlockedUpgrades[InUpgrade] : 0; }

int32 USaveSlot::GetLevelDamageIncrease(const bool bOneLower) { return PlayerLevelData.GetDamageIncrease(bOneLower ? CurrentLevel - 1 : CurrentLevel); }

void USaveSlot::AddUpgrade(const EUpgrade InUpgrade, const int32 InLevel)
{
	if (UnlockedUpgrades.Find(InUpgrade)) UnlockedUpgrades[InUpgrade] = InLevel;
	else UnlockedUpgrades.Add(InUpgrade, InLevel);
	GameInstance->GetSubsystem<UDelegateManager>()->ApplyUpgradeOnSelection.Broadcast(InUpgrade);
}

void USaveSlot::AddXp(const int32 InXp)
{
	CurrentXp += InXp;
	if (CurrentXp >= GetCurrentLevelNeededXp())
	{
		GameInstance->GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::LevelUp);
		CurrentXp -= GetCurrentLevelNeededXp();
		CurrentLevel++;
		GameInstance->GetSubsystem<UDelegateManager>()->OnLevelUp.Broadcast();
	}
}
