#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UpgradeManager.generated.h"

class UUpgradeData;
enum class EUpgrade : uint8;

UCLASS()
class POSSESSIONGAME_API UUpgradeManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	TArray<TObjectPtr<UUpgradeData>> GetRandomUpgrades();
	
	void Init(const TMap<EUpgrade, TObjectPtr<UUpgradeData>>& InUpgrades);
private:
	UPROPERTY()
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> Upgrades;

	void AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<TObjectPtr<UUpgradeData>>& ChosenUpgrades);
};
