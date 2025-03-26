#include "SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

int32 USaveSlot::GetLevelOfAnUpgrade(const EUpgrade InUpgrade) { return UnlockedUpgrades.Find(InUpgrade) ? UnlockedUpgrades[InUpgrade] : 0; }

void USaveSlot::AddUpgrade(const EUpgrade InUpgrade, const int32 InLevel)
{
	if (UnlockedUpgrades.Find(InUpgrade)) UnlockedUpgrades[InUpgrade] = InLevel;
	else UnlockedUpgrades.Add(InUpgrade, InLevel);
	GameInstance->GetSubsystem<UDelegateManager>()->ApplyUpgradeOnSelection.Broadcast(InUpgrade);
}
