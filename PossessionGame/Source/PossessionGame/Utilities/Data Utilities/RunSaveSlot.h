#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RunSaveSlot.generated.h"

class UPossessionGameInstance;
enum class EUpgrade : uint8;

USTRUCT()
struct POSSESSIONGAME_API FUpgradeChoicesAmountByLevel
{
	GENERATED_BODY()

	explicit FUpgradeChoicesAmountByLevel() {}
	explicit FUpgradeChoicesAmountByLevel(const int32 InChoice) { AddOrUpdate(InChoice); }

	UPROPERTY()
	TMap<int32, int32> Data;

	int32 GetChoicesAmount() const { return Data.Num(); }
	int32 GetLevelOfChoice(const int32 InChoice) const
	{
		if (const int32* FoundLevel = Data.Find(InChoice)) return *FoundLevel;
		return 0;
	}
	int32 GetCurrentLevelOfAnUpgrade() const
	{
		int32 Level = 0;
		for (const TPair<int32, int32>& Choice : Data) Level += Choice.Value;
		return Level;
	}
	
	void AddOrUpdate(const int32 InChoice)
	{
		if (int32* Existing = Data.Find(InChoice)) (*Existing)++;
		else Data.Add(InChoice, 1);
	}
};

UCLASS()
class POSSESSIONGAME_API URunSaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	int32 GetLevelOfChoice(const EUpgrade InUpgrade, const int32 InChoice);
	int32 GetLevelOfAnUpgrade(const EUpgrade InUpgrade);
	const TMap<int32, int32>* GetUpgradeChoicesData(const EUpgrade InUpgrade);
	
	void AddUpgrade(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const EUpgrade InUpgrade, const int32 InChoice);
	void SaveSlot();
	void SetupSlot(const FString& InSlotName) { SlotName = InSlotName; }
private:
	UPROPERTY()
	int32 SaveVersion = 1;
	UPROPERTY()
	FString SlotName;
	UPROPERTY()
	TMap<EUpgrade, FUpgradeChoicesAmountByLevel> UnlockedUpgrades;
};
