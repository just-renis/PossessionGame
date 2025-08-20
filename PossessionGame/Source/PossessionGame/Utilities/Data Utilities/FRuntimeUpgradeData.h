#pragma once

#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "FRuntimeUpgradeData.generated.h"

enum class EUpgrade : uint8;
struct FUpgradeChoicePair;
enum class EUpgradeChoice : uint8;

USTRUCT()
struct FRuntimeUpgradeData
{
	GENERATED_BODY()

	FRuntimeUpgradeData() : Character(), UpgradeName() { }

	explicit FRuntimeUpgradeData(const TObjectPtr<UUpgradeData> InUpgradeData)
	{
		Upgrades = InUpgradeData->GetUpgrades();
		PlayerCardIcon = InUpgradeData->GetPlayerCardIcon();
		Character = InUpgradeData->GetCharacter();
		UpgradeName = InUpgradeData->GetUpgradeName();
	}
	int32 GetMaxRarityChoicesCount(const EUpgradeChoice InUpgradeChoice) const
	{
		if (const FUpgradeChoices* UpgradeChoices = Upgrades.UpgradesChoices.Find(InUpgradeChoice)) return UpgradeChoices->UpgradeChoicesPairs.Num();
		return 0;
	}
	ECharacterType GetCharacter() const { return Character; }
	EUpgrade GetUpgradeName() const { return UpgradeName; }
	TObjectPtr<UTexture2D> GetPlayerCardIcon() { return PlayerCardIcon; }
	FUpgradeChoicePair GetRandomUpgradeChoice(const EUpgradeChoice InUpgradeChoice, const TArray<FUpgradeChoicePair>& OccupiedUpgradeChoices) const
	{
		return Upgrades.GetRandomUpgradeChoice(InUpgradeChoice, OccupiedUpgradeChoices);
	}
	TMap<EUpgradeChoice, FUpgradeChoices>* GetUpgradeChoices() { return &Upgrades.UpgradesChoices; }
private:
	UPROPERTY()
	ECharacterType Character;
	UPROPERTY()
	EUpgrade UpgradeName;
	UPROPERTY()
	TObjectPtr<UTexture2D> PlayerCardIcon;
	UPROPERTY()
	FUpgrades Upgrades;
};
