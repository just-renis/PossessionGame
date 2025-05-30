#pragma once

#include "CoreMinimal.h"
#include "UpgradeData.generated.h"

enum class ECharacterType : uint8;
enum class EUpgradeAttribute : uint8;
enum class EUpgrade : uint8;

USTRUCT()
struct POSSESSIONGAME_API FUpgradeAttributes
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<EUpgradeAttribute, float> Upgrades;
};

USTRUCT()
struct POSSESSIONGAME_API FUpgradeChoices
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FUpgradeAttributes> UpgradeChoices;
};

USTRUCT()
struct POSSESSIONGAME_API FUpgrades
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FUpgradeChoices> PlayerUpgrades;
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FUpgradeAttributes> AIUpgrades;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxLevel;

	int32 GetMaxLevel() const { return MaxLevel; }
};

UCLASS()
class POSSESSIONGAME_API UUpgradeData : public UDataAsset
{
	GENERATED_BODY()
public:
	int32 GetMaxLevel() const { return Upgrades.GetMaxLevel(); }
	TObjectPtr<UTexture2D> GetPlayerCardIcon() { return PlayerCardIcon; }
	TObjectPtr<UTexture2D> GetEnemyCardIcon() { return EnemyCardIcon; }
	EUpgrade GetUpgradeName() const { return UpgradeName; }
	ECharacterType GetCharacter() const { return Character; }
	FUpgradeAttributes GetAttributesForLevel(const bool bIsPlayer, const int32 InLevel, const int32 InChoice) const;
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EUpgrade UpgradeName;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> PlayerCardIcon;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> EnemyCardIcon;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	ECharacterType Character;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FUpgrades Upgrades;
};
