#pragma once

#include "CoreMinimal.h"
#include "FCharacterAbilities.h"
#include "FVariationUnlockData.h"
#include "GameFramework/SaveGame.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EExplanation.h"
#include "SaveSlot.generated.h"

enum class EExplanation : uint8;
class UAbility;
enum class EAbility : uint8;
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
	bool UnlockAbility(const ECharacterType InCharacter, const int32 InAbilityIndex);
	bool GetIsAbilityUnlocked(const ECharacterType InCharacter, const int32 InAbilityIndex);
	bool GetIsExplanationShown(const EExplanation InExplanationName) const;
	int32 GetXp() const { return CurrentXp; }
	int32 GetPlayerLevel() const { return CurrentLevel; }
	int32 GetCurrentLevelNeededXp() const;
	int32 GetLevelDamageIncrease(const int32 InLevel = -1) const;
	int32 GetLevelHpIncrease(const int32 InLevel = -1) const;
	int32 GetVariationUnlockProgress(const EVariation InVariationUnlockName) const;
	int32 GetVariationUnlockRequirement(const EVariation InVariationUnlockName) const;
	int32 GetCurrencyAmount() const { return CurrencyAmount; }
	int32 GetSkillPoints() const;
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	FCharacterAbilities GetCharacterChosenAbilities(const ECharacterType InCharacter);
	FCharacterAbilities GetCharacterUnlockedAbilities(const ECharacterType InCharacter);
	
	void AddCurrencyAmount(const int32 InCurrencyAmount);
	void SetTutorialFinished();
	void SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const FString& InSlotName);
	void AddXp(const TObjectPtr<UGameInstance>& InGameInstance, const int32 InXp);
	void ProgressVariationUnlock(const EVariation InVariationUnlockName, const int32 InAmount);
	void SaveSlot();
	void SetLaunchedOnce();
	void UnlockVariation(const EVariation InVariationUnlockName);
	void AddChosenAbilities(const ECharacterType InCharacter, const int32 InFirstAbilityIndex, const int32 InSecondAbilityIndex);
	void AddShownExplanation(const EExplanation InExplanationName);
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
	UPROPERTY()
	TSet<EExplanation> ShownExplanations;
	UPROPERTY()
	TMap<ECharacterType, FCharacterAbilities> CharacterChosenAbilities;
	UPROPERTY()
	TMap<ECharacterType, FCharacterAbilities> CharacterUnlockedAbilities;

	void UpdateSelectedVariation(const ECharacterType InCharacter, const EVariation InVariation);
	void VersionMigration();
};
