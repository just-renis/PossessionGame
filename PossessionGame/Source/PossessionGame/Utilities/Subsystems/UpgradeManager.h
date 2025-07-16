#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "Subsystems/WorldSubsystem.h"
#include "UpgradeManager.generated.h"

class URunSaveSlot;
enum class EPoolObject : uint8;
enum class EAbility : uint8;
enum class EUpgradeAttribute : uint8;
enum class ECharacterType : uint8;
class UUpgradeData;
enum class EUpgrade : uint8;

UCLASS()
class POSSESSIONGAME_API UUpgradeManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	float AI_GetTotalUpgradeAttributeValue(const EUpgradeAttribute InUpgradeAttribute);
	
	float GetUpgradeAttributeValue(const bool bIsPlayer, const EPoolObject InProjectileName, const EUpgradeAttribute InUpgradeAttribute);
	float GetUpgradeAttributeValue(const bool bIsPlayer, const EAbility InAbilityName, const EUpgradeAttribute InUpgradeAttribute);
	TObjectPtr<UUpgradeData> GetProjectileUpgradeData(const EPoolObject InProjectileName);
	TObjectPtr<UUpgradeData> GetAbilityUpgradeData(const EAbility InAbilityName);
	TArray<TObjectPtr<UUpgradeData>> GetRandomUpgrades();
	
	void Init(const TMap<EUpgrade, TObjectPtr<UUpgradeData>>& InUpgrades);
private:
	UPROPERTY()
	TObjectPtr<URunSaveSlot> RunSaveSlot;
	UPROPERTY()
	TMap<EAbility, EUpgrade> AbilityToUpgradeMap;
	UPROPERTY()
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> Upgrades;

	void AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<TObjectPtr<UUpgradeData>>& ChosenUpgrades);
};
