#include "UpgradeManager.h"
#include "SaveSlotManager.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeChoice.h"
#include "PossessionGame/Utilities/Data Utilities/FRuntimeUpgradeData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"

TArray<FRuntimeUpgradeData> UUpgradeManager::GetRandomUpgrades()
{
	TArray<FRuntimeUpgradeData> ChosenUpgrades;
	TArray<EUpgrade> UpgradeKeys;
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> ChoosableUpgrades = Upgrades;
	FilterSelectedCharacterAbilities(ChoosableUpgrades);
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
		{EAbility::RangerAbility3, EUpgrade::RangerAbility3},
		{EAbility::TricksterAbility1, EUpgrade::TricksterAbility1},
		{EAbility::TricksterAbility2, EUpgrade::TricksterAbility2},
		{EAbility::TricksterAbility3, EUpgrade::TricksterAbility3},
		{EAbility::GuardianAbility1, EUpgrade::GuardianAbility1},
		{EAbility::GuardianAbility2, EUpgrade::GuardianAbility2},
		{EAbility::GuardianAbility3, EUpgrade::GuardianAbility3},
		{EAbility::BreacherAbility1, EUpgrade::BreacherAbility1},
		{EAbility::BreacherAbility2, EUpgrade::BreacherAbility2},
		{EAbility::BreacherAbility3, EUpgrade::BreacherAbility3},
		{EAbility::TankAbility1, EUpgrade::TankAbility1},
		{EAbility::TankAbility2, EUpgrade::TankAbility2},
		{EAbility::WraithAbility1, EUpgrade::WraithAbility1},
		{EAbility::WraithAbility2, EUpgrade::WraithAbility2},
		{EAbility::WraithAbility3, EUpgrade::WraithAbility3}
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

void UUpgradeManager::FilterSelectedCharacterAbilities(TMap<EUpgrade, TObjectPtr<UUpgradeData>>& InChoosableUpgrades) const
{
	const TArray<EUpgrade> AvailableUpgrades = RunSaveSlot->GetAvailableUpgrades();
	TArray<EUpgrade> KeysToCheck;
	InChoosableUpgrades.GetKeys(KeysToCheck);
	for (int32 i = 0; i < KeysToCheck.Num(); i++) if (!AvailableUpgrades.Contains(KeysToCheck[i])) InChoosableUpgrades.Remove(KeysToCheck[i]);
}
