#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UpgradeManager.generated.h"

struct FRuntimeUpgradeData;
class URunSaveSlot;
enum class EPoolObject : uint8;
enum class EAbility : uint8;
class UUpgradeData;
enum class EUpgrade : uint8;

UCLASS()
class POSSESSIONGAME_API UUpgradeManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	TArray<FRuntimeUpgradeData> GetRandomUpgrades();
	
	void Init(const TMap<EUpgrade, TObjectPtr<UUpgradeData>>& InUpgrades);
private:
	UPROPERTY()
	TObjectPtr<URunSaveSlot> RunSaveSlot;
	UPROPERTY()
	TMap<EAbility, EUpgrade> AbilityToUpgradeMap;
	UPROPERTY()
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> Upgrades;

	void AddRandomlyChosenUpgrade(TArray<EUpgrade>& UpgradeKeys, TArray<FRuntimeUpgradeData>& ChosenUpgrades);
};
