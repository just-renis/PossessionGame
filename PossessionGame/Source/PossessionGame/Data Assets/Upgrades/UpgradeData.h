#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EUpgradeChoice.h"
#include "UpgradeData.generated.h"

enum class EUpgradeChoice : uint8;
enum class ECharacterType : uint8;
enum class EUpgradeAttribute : uint8;
enum class EUpgrade : uint8;

USTRUCT()
struct POSSESSIONGAME_API FUpgradeAttributes
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<EUpgradeAttribute, float> UpgradeAttributes;
};

USTRUCT()
struct POSSESSIONGAME_API FUpgradeChoicePair
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 Id = -1;
	UPROPERTY(EditDefaultsOnly)
	FUpgradeAttributes PlayerAttributes;
	UPROPERTY(EditDefaultsOnly)
	FUpgradeAttributes EnemyAttributes;

	bool IsEmpty() const { return Id == -1; }
	bool operator==(const FUpgradeChoicePair& Other) const { return Id == Other.Id; }
};

USTRUCT()
struct POSSESSIONGAME_API FUpgradeChoices
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FUpgradeChoicePair> UpgradeChoicesPairs;
};

USTRUCT()
struct POSSESSIONGAME_API FUpgrades
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<EUpgradeChoice, FUpgradeChoices> UpgradesChoices;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxLevel = 3;

	int32 GetMaxLevel() const { return MaxLevel; }
	FUpgradeChoicePair GetRandomUpgradeChoice(const EUpgradeChoice InUpgradeChoice, const TArray<FUpgradeChoicePair>& OccupiedUpgradeChoices) const
	{
		const FUpgradeChoices* UpgradeChoicesData = UpgradesChoices.Find(InUpgradeChoice);
		if (!UpgradeChoicesData) return FUpgradeChoicePair();
		TArray<FUpgradeChoicePair> Filtered;
		for (const FUpgradeChoicePair& Pair : UpgradeChoicesData->UpgradeChoicesPairs) if (!OccupiedUpgradeChoices.Contains(Pair)) Filtered.Add(Pair);
		if (Filtered.IsEmpty()) return FUpgradeChoicePair();
		return Filtered[FMath::RandRange(0, Filtered.Num() - 1)];
	}
};

UCLASS()
class POSSESSIONGAME_API UUpgradeData : public UDataAsset
{
	GENERATED_BODY()
public:
	int32 GetMaxLevel() const { return Upgrades.GetMaxLevel(); }
	TObjectPtr<UTexture2D> GetPlayerCardIcon() { return PlayerCardIcon; }
	EUpgrade GetUpgradeName() const { return UpgradeName; }
	ECharacterType GetCharacter() const { return Character; }
	FUpgrades GetUpgrades() const { return Upgrades; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EUpgrade UpgradeName;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> PlayerCardIcon;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	ECharacterType Character;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FUpgrades Upgrades;
};
