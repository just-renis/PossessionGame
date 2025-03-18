#include "UpgradeManager.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/PossessionGameInstance.h"

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

float UUpgradeManager::GetUpgradeAttributeValue(const bool bIsPlayer, const ECharacterType InCharacterName, const EUpgradeAttribute InUpgradeAttribute)
{
	if (const TObjectPtr<UUpgradeData> UpgradeData = GetCharacterBaseUpgradeData(InCharacterName))
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
		case EPoolObject::RangerProjectile: return Upgrades[EUpgrade::RangerBaseStat];
		case EPoolObject::RangerGrenadeLauncherProjectile: return Upgrades[EUpgrade::RangerAbility1];
		default: return nullptr;
	}
}

TObjectPtr<UUpgradeData> UUpgradeManager::GetAbilityUpgradeData(const EAbility InAbilityName)
{
	switch (InAbilityName)
	{
		case EAbility::RangerAbility1: return Upgrades[EUpgrade::RangerAbility1];
		case EAbility::RangerAbility2: return Upgrades[EUpgrade::RangerAbility2];
		case EAbility::RangerAbility3: return Upgrades[EUpgrade::RangerAbility3];
		default: return nullptr;
	}
}

TObjectPtr<UUpgradeData> UUpgradeManager::GetCharacterBaseUpgradeData(const ECharacterType InCharacterName)
{
	switch (InCharacterName)
	{
		case ECharacterType::Ranger: return Upgrades[EUpgrade::RangerBaseStat];
		default: return nullptr;
	}
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
	GameInstance = Cast<UPossessionGameInstance>(GetWorld()->GetGameInstance());
}

void UUpgradeManager::AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<TObjectPtr<UUpgradeData>>& ChosenUpgrades)
{
	const int32 RandomIndex = FMath::RandRange(0, UpgradeKeys.Num() - 1);
	ChosenUpgrades.Add(Upgrades[UpgradeKeys[RandomIndex]]);
	UpgradeKeys.RemoveAt(RandomIndex);
}
