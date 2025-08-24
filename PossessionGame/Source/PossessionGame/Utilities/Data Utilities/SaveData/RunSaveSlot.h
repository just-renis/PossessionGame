#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Utilities/Data Utilities/FSpecialUpgradesContainer.h"
#include "RunSaveSlot.generated.h"

enum class EPoolObject : uint8;
enum class EAbility : uint8;
enum class EUpgrade : uint8;
enum class ECharacterType : uint8;
enum class EUpgradeAttribute : uint8;
enum class EVariation : uint8;
class UPossessionGameInstance;

USTRUCT()
struct POSSESSIONGAME_API FUpgradeAttributeValues
{
	GENERATED_BODY()

	UPROPERTY()
	int32 CurrentLevel = 0;
	UPROPERTY()
	TMap<EUpgradeAttribute, float> EnemyUpgrades;
	UPROPERTY()
	TMap<EUpgradeAttribute, float> PlayerUpgrades;

	void AddUpgrade(const FUpgradeChoicePair& InUpgradeChoicePair)
	{
		CurrentLevel++;
		for (const TPair<EUpgradeAttribute, float>& UpgradeAttribute : InUpgradeChoicePair.PlayerAttributes.UpgradeAttributes) PlayerUpgrades.FindOrAdd(UpgradeAttribute.Key) += UpgradeAttribute.Value;
		for (const TPair<EUpgradeAttribute, float>& UpgradeAttribute : InUpgradeChoicePair.EnemyAttributes.UpgradeAttributes) EnemyUpgrades.FindOrAdd(UpgradeAttribute.Key) += UpgradeAttribute.Value;
	}
};

USTRUCT()
struct POSSESSIONGAME_API FUpgradeChoicesIds
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<int32> Ids;
};

UCLASS()
class POSSESSIONGAME_API URunSaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	bool IsSpecialUpgradeAvailable(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialUpgradeAttribute);
	float GetUpgradeAttributeValues(const EUpgrade InUpgradeName, const EUpgradeAttribute InUpgradeAttribute, const bool bIsPlayer);
	float GetUpgradeAttributeValues(const EAbility InAbilityName, const EUpgradeAttribute InUpgradeAttribute, const bool bIsPlayer);
	float GetUpgradeAttributeValues(const EPoolObject InPoolObject, const EUpgradeAttribute InUpgradeAttribute, const bool bIsPlayer, const bool bForAbilityOnly = false);
	float GetHpUpgradesForEnemy(const ECharacterType InCharacterType);
	int32 GetSecretBoost(const EVariation InVariation);
	int32 GetCurrentLevel(const EUpgrade InUpgradeName);
	int32 GetCurrentLevelSumOfCharacterAbilities(const ECharacterType InCharacterName);
	FString GetSlotName() { return SlotName; }
	TSet<int32> GetUpgradeChoicesIds(const EUpgrade InUpgradeName);
	
	void AddUpgrade(const TObjectPtr<UGameInstance>& InGameInstance, const EUpgrade InUpgrade, const FUpgradeChoicePair& InUpgradeChoicePair, const EUpgradeAttribute InSpecialAttribute);
	void SaveSlot();
	void SetupSlot(const TObjectPtr<UGameInstance>& InGameInstance, const FString& InSlotName);
	void AddSecretCrateBoost(const EVariation InVariation, const int32 InPercentage);
private:
	UPROPERTY()
	int32 SaveVersion = 1;
	UPROPERTY()
	FString SlotName;
	UPROPERTY()
	TMap<EUpgrade, FUpgradeChoicesIds> ChosenUpgradeChoices;
	UPROPERTY()
	TMap<EUpgrade, FUpgradeAttributeValues> UnlockedUpgrades;
	UPROPERTY()
	TMap<EUpgrade, FSpecialUpgradesContainer> SpecialUpgrades;
	UPROPERTY()
	TMap<EVariation, int32> SecretCrateBoosts;

	float GetUpgradeAttributeValuesForEnemy(const EPoolObject InPoolObject, const EUpgradeAttribute InUpgradeAttribute, const bool bForAbilityOnly);
	EUpgrade GetUpgradeByAbilityName(const EAbility InAbilityName);
	EUpgrade GetUpgradeByPoolObjectName(const EPoolObject InPoolObjectName);
	TArray<EUpgrade> GetUpgradesByPoolObjectName(const EPoolObject InPoolObjectName, const bool bForAbilityOnly);
	TArray<EUpgrade> GetHpUpgradesByCharacterName(const ECharacterType InCharacterName);
	
	void VersionMigration();
};
