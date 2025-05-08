#pragma once

#include "CoreMinimal.h"
#include "FPlayerLevelData.h"
#include "FVariationUnlockData.h"
#include "GameFramework/SaveGame.h"
#include "SaveSlot.generated.h"

class UPossessionGameInstance;
enum class EUpgrade : uint8;

UCLASS()
class POSSESSIONGAME_API USaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	bool GetIsTutorialFinished() const { return bTutorialFinished; }
	bool GetIsVariationUnlocked(const EVariationUnlock InVariationUnlockName) const;
	int32 GetXp() const { return CurrentXp; }
	int32 GetCurrentLevelNeededXp() const { return 10 + CurrentLevel * 5; }
	int32 GetLevelOfAnUpgrade(const EUpgrade InUpgrade);
	int32 GetLevelDamageIncrease(const int32 InLevel = -1);
	int32 GetVariationUnlockProgress(const EVariationUnlock InVariationUnlockName) const;
	int32 GetVariationUnlockRequirement(const EVariationUnlock InVariationUnlockName) const;
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	
	void SetTutorialFinished() { bTutorialFinished = true; }
	void SetGameInstance(const TObjectPtr<UPossessionGameInstance>& InGameInstance) { GameInstance = InGameInstance; }
	void AddUpgrade(const EUpgrade InUpgrade, const int32 InLevel);
	void AddXp(const int32 InXp);
	void ProgressVariationUnlock(const EVariationUnlock InVariationUnlockName);
private:
	FPlayerLevelData PlayerLevelData;
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
	TMap<EUpgrade, int32> UnlockedUpgrades;
};
