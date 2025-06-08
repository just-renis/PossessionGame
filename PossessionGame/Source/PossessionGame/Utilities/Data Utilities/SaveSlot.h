#pragma once

#include "CoreMinimal.h"
#include "FVariationUnlockData.h"
#include "GameFramework/SaveGame.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "SaveSlot.generated.h"

class UPlayerLevelData;
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
class POSSESSIONGAME_API USaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	bool GetIsTutorialFinished() const { return bTutorialFinished; }
	bool GetIsVariationUnlocked(const EVariationUnlock InVariationUnlockName) const;
	int32 GetXp() const { return CurrentXp; }
	int32 GetCurrentLevelNeededXp() const;
	int32 GetLevelDamageIncrease(const int32 InLevel = -1) const;
	int32 GetLevelHpIncrease(const int32 InLevel = -1) const;
	int32 GetVariationUnlockProgress(const EVariationUnlock InVariationUnlockName) const;
	int32 GetVariationUnlockRequirement(const EVariationUnlock InVariationUnlockName) const;
	int32 GetLevelOfChoice(const EUpgrade InUpgrade, const int32 InChoice);
	int32 GetLevelOfAnUpgrade(const EUpgrade InUpgrade);
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	const TMap<int32, int32>* GetUpgradeChoicesData(const EUpgrade InUpgrade);
	
	void SetTutorialFinished();
	void SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance);
	void AddUpgrade(const EUpgrade InUpgrade, const int32 InChoice);
	void AddXp(const int32 InXp);
	void ProgressVariationUnlock(const EVariationUnlock InVariationUnlockName);
	void SaveSlot();
private:
	UPROPERTY()
	TObjectPtr<UPlayerLevelData> PlayerLevelData;
	UPROPERTY()
	FVariationUnlockData VariationUnlockData;
	UPROPERTY()
	bool bTutorialFinished = false;
	UPROPERTY()
	int32 CurrentXp = 0;
	UPROPERTY()
	int32 CurrentLevel = 0;
	UPROPERTY()
	TObjectPtr<UPossessionGameInstance> GameInstance;
	UPROPERTY()
	TMap<EUpgrade, FUpgradeChoicesAmountByLevel> UnlockedUpgrades;

	void UpdateSelectedVariation(const ECharacterType InCharacter, const EVariation InVariation);
};
