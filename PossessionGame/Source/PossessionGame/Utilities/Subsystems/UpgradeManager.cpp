#include "UpgradeManager.h"
#include "SaveSlotManager.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"

float UUpgradeManager::AI_GetTotalUpgradeAttributeValue(const EUpgradeAttribute InUpgradeAttribute)
{
	float Value = 0.f;
	for (const TPair<EUpgrade, TObjectPtr<UUpgradeData>> Upgrade : Upgrades)
	{
		if (InUpgradeAttribute == EUpgradeAttribute::FlatHealth) Value += 5.f * RunSaveSlot->GetLevelOfAnUpgrade(Upgrade.Key);
		else if (InUpgradeAttribute == EUpgradeAttribute::FlatDamage) Value += 1.f * RunSaveSlot->GetLevelOfAnUpgrade(Upgrade.Key);
	}
	return Value;
}

float UUpgradeManager::GetUpgradeAttributeValue(const bool bIsPlayer, const EPoolObject InProjectileName, const EUpgradeAttribute InUpgradeAttribute)
{
	if (const TObjectPtr<UUpgradeData> UpgradeData = GetProjectileUpgradeData(InProjectileName))
	{
		float AttributeValue = 0.f;
		if (const TMap<int32, int32>* UpgradeChoicesData = RunSaveSlot->GetUpgradeChoicesData(UpgradeData->GetUpgradeName()))
		{
			for (const TPair<int32, int32>& ChoicePair : *UpgradeChoicesData)
			{
				const TMap<EUpgradeAttribute, float>& UpgradeAttributes = UpgradeData->GetAttributesForLevel(bIsPlayer, ChoicePair.Value, ChoicePair.Key).Upgrades;
				if (const float* Value = UpgradeAttributes.Find(InUpgradeAttribute)) AttributeValue += *Value;
			}
		}
		return AttributeValue;
	}
	return 0.f;
}

float UUpgradeManager::GetUpgradeAttributeValue(const bool bIsPlayer, const EAbility InAbilityName, const EUpgradeAttribute InUpgradeAttribute)
{
	if (const TObjectPtr<UUpgradeData> UpgradeData = GetAbilityUpgradeData(InAbilityName))
	{
		float AttributeValue = 0.f;
		if (const TMap<int32, int32>* UpgradeChoicesData = RunSaveSlot->GetUpgradeChoicesData(UpgradeData->GetUpgradeName()))
		{
			for (const TPair<int32, int32>& ChoicePair : *UpgradeChoicesData)
			{
				const TMap<EUpgradeAttribute, float>& UpgradeAttributes = UpgradeData->GetAttributesForLevel(bIsPlayer, ChoicePair.Value, ChoicePair.Key).Upgrades;
				if (const float* Value = UpgradeAttributes.Find(InUpgradeAttribute)) AttributeValue += *Value;
			}
		}
		return AttributeValue;
	}
	return 0.f;
}

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

TArray<TObjectPtr<UUpgradeData>> UUpgradeManager::GetRandomUpgrades()
{
	TArray<TObjectPtr<UUpgradeData>> ChosenUpgrades;
	TArray<EUpgrade> UpgradeKeys;
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> ChoosableUpgrades = Upgrades;
	for (const TPair<EUpgrade, TObjectPtr<UUpgradeData>> UpgradeData : ChoosableUpgrades)
	{
		if (UpgradeData.Value->GetMaxLevel() <= RunSaveSlot->GetLevelOfAnUpgrade(UpgradeData.Key)) UpgradeKeys.Add(UpgradeData.Key);
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
		{EAbility::TricksterAbility1, EUpgrade::TricksterAbility1},
		{EAbility::TricksterAbility2, EUpgrade::TricksterAbility2},
		{EAbility::GuardianAbility1, EUpgrade::GuardianAbility1},
		{EAbility::GuardianAbility2, EUpgrade::GuardianAbility2}
	};
	RunSaveSlot = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
}

void UUpgradeManager::AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<TObjectPtr<UUpgradeData>>& ChosenUpgrades)
{
	const int32 RandomIndex = FMath::RandRange(0, UpgradeKeys.Num() - 1);
	ChosenUpgrades.Add(Upgrades[UpgradeKeys[RandomIndex]]);
	UpgradeKeys.RemoveAt(RandomIndex);
}
