#pragma once

#include "CoreMinimal.h"
#include "UpgradeData.generated.h"

enum class ECharacterType : uint8;
enum class EUpgradeAttribute : uint8;
enum class EUpgrade : uint8;

USTRUCT()
struct POSSESSIONGAME_API FLevelUpgradeData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<EUpgradeAttribute, float> Upgrades;
};

USTRUCT()
struct POSSESSIONGAME_API FUpgradeAttributeValues
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FLevelUpgradeData> PlayerUpgrades;
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, FLevelUpgradeData> AIUpgrades;

	int32 GetMaxLevel() const { return PlayerUpgrades.Num(); }
};

UCLASS()
class POSSESSIONGAME_API UUpgradeData : public UDataAsset
{
	GENERATED_BODY()
public:
	int32 GetMaxLevel() const { return UpgradeAttributeValues.GetMaxLevel(); }
	TObjectPtr<UTexture2D> GetPlayerCardIcon() { return PlayerCardIcon; }
	TObjectPtr<UTexture2D> GetEnemyCardIcon() { return EnemyCardIcon; }
	EUpgrade GetUpgradeName() const { return UpgradeName; }
	ECharacterType GetCharacter() const { return Character; }
	FLevelUpgradeData GetAttributesForLevel(const bool bIsPlayer, const int32 InLevel) const;
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
	FUpgradeAttributeValues UpgradeAttributeValues;
};
