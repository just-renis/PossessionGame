#include "UpgradeManager.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"

float UUpgradeManager::GetUpgradeAttributeValue(const bool bIsPlayer, const EPoolObject InProjectileName, const EUpgradeAttribute InUpgradeAttribute)
{
	if (const TObjectPtr<UUpgradeData> UpgradeData = GetProjectileUpgradeData(InProjectileName))
	{
		const TObjectPtr<USaveSlot> SaveSlot = GameInstance->GetSaveSlot();
		const TMap<EUpgradeAttribute, float> UpgradeAttributes = UpgradeData->GetAttributesForLevel(bIsPlayer, SaveSlot->GetLevelOfAnUpgrade(UpgradeData->GetUpgradeName())).Upgrades;
		if (UpgradeAttributes.Find(InUpgradeAttribute)) return UpgradeAttributes[InUpgradeAttribute];
	}
	return 0.f;
}

float UUpgradeManager::GetUpgradeAttributeValue(const bool bIsPlayer, const EAbility InAbilityName, const EUpgradeAttribute InUpgradeAttribute)
{
	if (const TObjectPtr<UUpgradeData> UpgradeData = GetAbilityUpgradeData(InAbilityName))
	{
		const TObjectPtr<USaveSlot> SaveSlot = GameInstance->GetSaveSlot();
		const TMap<EUpgradeAttribute, float> UpgradeAttributes = UpgradeData->GetAttributesForLevel(bIsPlayer, SaveSlot->GetLevelOfAnUpgrade(UpgradeData->GetUpgradeName())).Upgrades;
		if (UpgradeAttributes.Find(InUpgradeAttribute)) return UpgradeAttributes[InUpgradeAttribute];
	}
	return 0.f;
}

TObjectPtr<UUpgradeData> UUpgradeManager::GetProjectileUpgradeData(const EPoolObject InProjectileName)
{
	switch (InProjectileName)
	{
		case EPoolObject::RangerProjectile: return Upgrades.Contains(EUpgrade::RangerBaseStat) ? Upgrades[EUpgrade::RangerBaseStat] : nullptr;
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
		return Upgrades.Contains(*FoundUpgrade) ? Upgrades[*FoundUpgrade] : nullptr;
	}
	return nullptr;
}

TArray<TObjectPtr<UUpgradeData>> UUpgradeManager::GetRandomUpgrades()
{
	TArray<TObjectPtr<UUpgradeData>> ChosenUpgrades;
	TArray<EUpgrade> UpgradeKeys;
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> ChoosableUpgrades = Upgrades;
	const TObjectPtr<USaveSlot> SaveSlot = GameInstance->GetSaveSlot();
	for (const TPair<EUpgrade, TObjectPtr<UUpgradeData>> UpgradeData : ChoosableUpgrades)
	{
		if (UpgradeData.Value->GetMaxLevel() <= SaveSlot->GetLevelOfAnUpgrade(UpgradeData.Key)) UpgradeKeys.Add(UpgradeData.Key);
	}
	for (const EUpgrade& UpgradeKey : UpgradeKeys) ChoosableUpgrades.Remove(UpgradeKey);
	UpgradeKeys.Empty();
	ChoosableUpgrades.GetKeys(UpgradeKeys);
	while (!UpgradeKeys.IsEmpty() && ChosenUpgrades.Num() < 2) AddRandomlyChosenUpgrade(UpgradeKeys, ChosenUpgrades);
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
		{EAbility::TricksterAbility2, EUpgrade::TricksterAbility2}
	};
	GameInstance = Cast<UPossessionGameInstance>(GetWorld()->GetGameInstance());
}

void UUpgradeManager::AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<TObjectPtr<UUpgradeData>>& ChosenUpgrades)
{
	const int32 RandomIndex = FMath::RandRange(0, UpgradeKeys.Num() - 1);
	ChosenUpgrades.Add(Upgrades[UpgradeKeys[RandomIndex]]);
	UpgradeKeys.RemoveAt(RandomIndex);
}
