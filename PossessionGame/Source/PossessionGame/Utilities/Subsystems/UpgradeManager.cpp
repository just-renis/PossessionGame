#include "UpgradeManager.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/PossessionGameInstance.h"

TArray<TObjectPtr<UUpgradeData>> UUpgradeManager::GetRandomUpgrades()
{
	TArray<TObjectPtr<UUpgradeData>> ChosenUpgrades;
	TArray<EUpgrade> UpgradeKeys;
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetWorld()->GetGameInstance())->GetSaveSlot();
	for (const TPair<EUpgrade, TObjectPtr<UUpgradeData>> UpgradeData : Upgrades)
	{
		if (UpgradeData.Value->GetMaxLevel() <= SaveSlot->GetLevelOfAnUpgrade(UpgradeData.Key)) UpgradeKeys.Add(UpgradeData.Key);
	}
	for (const EUpgrade& UpgradeKey : UpgradeKeys) Upgrades.Remove(UpgradeKey);
	UpgradeKeys.Empty();
	Upgrades.GetKeys(UpgradeKeys);
	while (!UpgradeKeys.IsEmpty() && ChosenUpgrades.Num() < 2) AddRandomlyChosenUpgrade(UpgradeKeys, ChosenUpgrades);
	return ChosenUpgrades;
}

void UUpgradeManager::Init(const TMap<EUpgrade, TObjectPtr<UUpgradeData>>& InUpgrades) { Upgrades = InUpgrades; }

void UUpgradeManager::AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<TObjectPtr<UUpgradeData>>& ChosenUpgrades)
{
	const int32 RandomIndex = FMath::RandRange(0, UpgradeKeys.Num() - 1);
	ChosenUpgrades.Add(Upgrades[UpgradeKeys[RandomIndex]]);
	UpgradeKeys.RemoveAt(RandomIndex);
}
