#include "UpgradeManager.h"
#include "SaveSlotManager.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeChoice.h"
#include "PossessionGame/Utilities/Data Utilities/FRuntimeUpgradeData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"

TObjectPtr<UUpgradeData> UUpgradeManager::GetProjectileUpgradeData(const EPoolObject InProjectileName)
{
	switch (InProjectileName)
	{
		case EPoolObject::RangerGrenadeLauncherProjectile: return Upgrades.Contains(EUpgrade::RangerAbility1) ? Upgrades[EUpgrade::RangerAbility1] : nullptr;
		case EPoolObject::TricksterPoisonProjectile: return Upgrades.Contains(EUpgrade::TricksterAbility1) ? Upgrades[EUpgrade::TricksterAbility1] : nullptr;
		case EPoolObject::TricksterEmpProjectile: return Upgrades.Contains(EUpgrade::TricksterAbility2) ? Upgrades[EUpgrade::TricksterAbility2] : nullptr;
		default: return nullptr;
	}
}

TObjectPtr<UUpgradeData> UUpgradeManager::GetAbilityUpgradeData(const EAbility InAbilityName)
{
	if (const EUpgrade* FoundUpgrade = AbilityToUpgradeMap.Find(InAbilityName))
	{
		if (TObjectPtr<UUpgradeData>* FoundData = Upgrades.Find(*FoundUpgrade)) return *FoundData;
	}
	return nullptr;
}

TArray<FRuntimeUpgradeData> UUpgradeManager::GetRandomUpgrades()
{
	TArray<FRuntimeUpgradeData> ChosenUpgrades;
	TArray<EUpgrade> UpgradeKeys;
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> ChoosableUpgrades = Upgrades;
	for (const TPair<EUpgrade, TObjectPtr<UUpgradeData>> UpgradeData : ChoosableUpgrades)
	{
		if (UpgradeData.Value->GetMaxLevel() <= RunSaveSlot->GetCurrentLevel(UpgradeData.Key)) UpgradeKeys.Add(UpgradeData.Key);
	}
	for (const EUpgrade& UpgradeKey : UpgradeKeys) ChoosableUpgrades.Remove(UpgradeKey);
	UpgradeKeys.Empty();
	ChoosableUpgrades.GetKeys(UpgradeKeys);
	while (!UpgradeKeys.IsEmpty() && ChosenUpgrades.Num() < 4) AddRandomlyChosenUpgrade(UpgradeKeys, ChosenUpgrades);
	return ChosenUpgrades;
}

void UUpgradeManager::Init(const TMap<EUpgrade, TObjectPtr<UUpgradeData>>& InUpgrades)
{
	Upgrades = InUpgrades;
	AbilityToUpgradeMap = {
		{EAbility::RangerAbility1, EUpgrade::RangerAbility1},
		{EAbility::RangerAbility2, EUpgrade::RangerAbility2},
		{EAbility::TricksterAbility1, EUpgrade::TricksterAbility1},
		{EAbility::TricksterAbility2, EUpgrade::TricksterAbility2},
		{EAbility::GuardianAbility1, EUpgrade::GuardianAbility1},
		{EAbility::GuardianAbility2, EUpgrade::GuardianAbility2}
	};
	RunSaveSlot = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
}

void UUpgradeManager::AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<FRuntimeUpgradeData>& ChosenUpgrades)
{
	const int32 RandomIndex = FMath::RandRange(0, UpgradeKeys.Num() - 1);
	FRuntimeUpgradeData UpgradeData = FRuntimeUpgradeData(Upgrades[UpgradeKeys[RandomIndex]]);
	const TSet<int32> UsedIds = RunSaveSlot->GetUpgradeChoicesIds(UpgradeData.GetUpgradeName());
	for (const EUpgradeChoice Choice : { EUpgradeChoice::Common, EUpgradeChoice::Rare, EUpgradeChoice::Legendary })
	{
		if (FUpgradeChoices* UpgradeChoicesData = UpgradeData.GetUpgradeChoices()->Find(Choice))
		{
			UpgradeChoicesData->UpgradeChoicesPairs.RemoveAll([&](const FUpgradeChoicePair& Pair) { return UsedIds.Contains(Pair.Id); });
		}
	}
	ChosenUpgrades.Add(UpgradeData);
	UpgradeKeys.RemoveAt(RandomIndex);
}
