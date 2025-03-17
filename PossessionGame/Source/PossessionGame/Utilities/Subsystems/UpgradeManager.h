#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "Subsystems/WorldSubsystem.h"
#include "UpgradeManager.generated.h"

class UPossessionGameInstance;
enum class EUpgradeAttribute : uint8;
enum class ECharacterType : uint8;
class UUpgradeData;
enum class EUpgrade : uint8;

UCLASS()
class POSSESSIONGAME_API UUpgradeManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	float GetUpgradeAttributeValue(const bool bIsPlayer, const ECharacterType InCharacterName, const EUpgradeAttribute InUpgradeAttribute);
	TObjectPtr<UUpgradeData> GetCharacterBaseUpgradeData(const ECharacterType InCharacterName);
	TArray<TObjectPtr<UUpgradeData>> GetRandomUpgrades();
	
	void Init(const TMap<EUpgrade, TObjectPtr<UUpgradeData>>& InUpgrades);
private:
	UPROPERTY()
	TObjectPtr<UPossessionGameInstance> GameInstance;
	UPROPERTY()
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> Upgrades;

	void AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<TObjectPtr<UUpgradeData>>& ChosenUpgrades);
};
