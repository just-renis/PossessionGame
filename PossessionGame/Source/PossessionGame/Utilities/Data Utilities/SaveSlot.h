#pragma once

#include "CoreMinimal.h"
#include "FVariationUnlockData.h"
#include "GameFramework/SaveGame.h"
#include "SaveSlot.generated.h"

class UDelegateManager;
class UPlayerLevelData;
class UPossessionGameInstance;

UCLASS()
class POSSESSIONGAME_API USaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	bool GetIsTutorialFinished() const { return bTutorialFinished; }
	bool GetIsVariationUnlocked(const EVariation InVariationUnlockName);
	bool GetIsVariationTaskCompleted(const EVariation InVariationUnlockName) const;
	bool GetIsFirstLaunch() const { return bIsFirstLaunch; }
	int32 GetXp() const { return CurrentXp; }
	int32 GetCurrentLevelNeededXp() const;
	int32 GetLevelDamageIncrease(const int32 InLevel = -1) const;
	int32 GetLevelHpIncrease(const int32 InLevel = -1) const;
	int32 GetVariationUnlockProgress(const EVariation InVariationUnlockName) const;
	int32 GetVariationUnlockRequirement(const EVariation InVariationUnlockName) const;
	int32 GetCurrencyAmount() const { return CurrencyAmount; }
	int32 GetVariationPrice(const EVariation InVariationUnlockName) const;
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	
	void AddCurrencyAmount(const int32 InCurrencyAmount) { CurrencyAmount += InCurrencyAmount; }
	void SetTutorialFinished();
	void SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const FString& InSlotName);
	void AddXp(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const int32 InXp);
	void ProgressVariationUnlock(const EVariation InVariationUnlockName, const int32 InAmount);
	void SaveSlot();
	void SetLaunchedOnce();
	void UnlockVariation(const EVariation InVariationUnlockName);
private:
	UPROPERTY()
	bool bIsFirstLaunch = true;
	UPROPERTY()
	bool bTutorialFinished = false;
	UPROPERTY()
	int32 CurrentXp = 0;
	UPROPERTY()
	int32 CurrentLevel = 0;
	UPROPERTY()
	int32 CurrencyAmount = 0;
	UPROPERTY()
	int32 SaveVersion = 1;
	UPROPERTY()
	FString SlotName;
	UPROPERTY(Transient)
	TObjectPtr<UPlayerLevelData> PlayerLevelData;
	UPROPERTY()
	FVariationUnlockData VariationUnlockData;

	void UpdateSelectedVariation(const ECharacterType InCharacter, const EVariation InVariation);
	void VersionMigration();
};
