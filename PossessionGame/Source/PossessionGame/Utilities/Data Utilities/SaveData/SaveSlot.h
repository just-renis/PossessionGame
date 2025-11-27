#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Data Utilities/FCharacterAbilities.h"
#include "PossessionGame/Utilities/Data Utilities/FVariationUnlockData.h"
#include "SaveSlot.generated.h"

class UDelegateManager;
enum class EExplanation : uint8;
enum class EVariation : uint8;
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
	bool UnlockAbility(const ECharacterType InCharacter, const int32 InAbilityIndex);
	bool GetIsAbilityUnlocked(const ECharacterType InCharacter, const int32 InAbilityIndex);
	bool GetIsExplanationShown(const EExplanation InExplanationName) const;
	int32 GetXp() const { return CurrentXp; }
	int32 GetPlayerLevel() const { return CurrentLevel; }
	int32 GetCurrentLevelNeededXp() const;
	int32 GetLevelDamageBoost() const;
	int32 GetLevelHpBoost() const;
	int32 GetVariationUnlockProgress(const EVariation InVariationUnlockName) const;
	int32 GetVariationUnlockRequirement(const EVariation InVariationUnlockName) const;
	int32 GetCurrencyAmount() const { return CurrencyAmount; }
	int32 GetSkillPoints() const;
	int32 GetArenaMaxTimeSurvivedSeconds() const { return ArenaMaxTimeSurvivedSeconds; }
	int32 GetLevelUpDamageIncrease() const;
	int32 GetLevelUpHpIncrease() const;
	EVariation GetSelectedVariation(const ECharacterType InCharacter) const;
	ECharacterType GetRandomSpawnableCharacter();
	FCharacterAbilities GetCharacterChosenAbilities(const ECharacterType InCharacter);
	FCharacterAbilities GetCharacterUnlockedAbilities(const ECharacterType InCharacter);
	FString GetSlotName() { return SlotName; }
	TArray<ECharacterType> GetAvailableCharacters() const { return SpawnableCharacters.Array(); }
	
	void AddCurrencyAmount(const int32 InCurrencyAmount);
	void SetTutorialFinished() { bTutorialFinished = true; }
	void AddSpawnableCharacter(const TObjectPtr<UDelegateManager>& InDelegateManager, const ECharacterType InCharacterName);
	void SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const FString& InSlotName);
	void AddXp(const int32 InXp);
	void ProgressVariationUnlock(const EVariation InVariationUnlockName, const int32 InAmount);
	void SaveSlot();
	void UnlockVariation(const EVariation InVariationUnlockName);
	void AddChosenAbilities(const ECharacterType InCharacter, const int32 InFirstAbilityIndex, const int32 InSecondAbilityIndex);
	void AddShownExplanation(const EExplanation InExplanationName);
	void AddArenaStats(const int32 InSurviveTime);
private:
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
	int32 ArenaMaxTimeSurvivedSeconds = 0;
	UPROPERTY()
	FString SlotName;
	UPROPERTY(Transient)
	TObjectPtr<UPlayerLevelData> PlayerLevelData;
	UPROPERTY(Transient)
	TObjectPtr<UPossessionGameInstance> GameInstance;
	UPROPERTY()
	FVariationUnlockData VariationUnlockData;
	UPROPERTY()
	TSet<ECharacterType> SpawnableCharacters;
	UPROPERTY()
	TSet<EExplanation> ShownExplanations;
	UPROPERTY()
	TMap<ECharacterType, FCharacterAbilities> CharacterChosenAbilities;
	UPROPERTY()
	TMap<ECharacterType, FCharacterAbilities> CharacterUnlockedAbilities;

	void UpdateSelectedVariation(const ECharacterType InCharacter, const EVariation InVariation);
	void VersionMigration();
	void AddDefaultAbilities();
};
