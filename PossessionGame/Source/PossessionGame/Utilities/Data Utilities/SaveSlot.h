#pragma once

#include "CoreMinimal.h"
#include "FVariationUnlockData.h"
#include "GameFramework/SaveGame.h"
#include "SaveSlot.generated.h"

class UPlayerLevelData;
class UPossessionGameInstance;

UCLASS()
class POSSESSIONGAME_API USaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	bool GetIsTutorialFinished() const { return bTutorialFinished; }
	bool GetIsVariationUnlocked(const EVariationUnlock InVariationUnlockName) const;
	bool GetIsFirstLaunch() const { return bIsFirstLaunch; }
	int32 GetXp() const { return CurrentXp; }
	int32 GetCurrentLevelNeededXp() const;
	int32 GetLevelDamageIncrease(const int32 InLevel = -1) const;
	int32 GetLevelHpIncrease(const int32 InLevel = -1) const;
	int32 GetVariationUnlockProgress(const EVariationUnlock InVariationUnlockName) const;
	int32 GetVariationUnlockRequirement(const EVariationUnlock InVariationUnlockName) const;
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	
	void SetTutorialFinished();
	void SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const FString& InSlotName);
	void AddXp(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const int32 InXp);
	void ProgressVariationUnlock(const EVariationUnlock InVariationUnlockName);
	void SaveSlot();
	void SetLaunchedOnce();
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
