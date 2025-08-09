#include "SaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/PlayerLevelData.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

constexpr int32 CURRENT_SAVE_VERSION = 1;

bool USaveSlot::GetIsVariationUnlocked(const EVariation InVariationUnlockName) { return VariationUnlockData.IsVariationUnlocked(InVariationUnlockName); }

bool USaveSlot::GetIsVariationTaskCompleted(const EVariation InVariationUnlockName) const { return VariationUnlockData.IsVariationTaskCompleted(InVariationUnlockName); }

bool USaveSlot::GetIsAbilityUnlocked(const ECharacterType InCharacter, const int32 InAbilityIndex)
{
	if (const FCharacterAbilities* CharacterAbilities = CharacterUnlockedAbilities.Find(InCharacter)) return CharacterAbilities->Abilities.Contains(InAbilityIndex);
	return false;
}

bool USaveSlot::GetIsExplanationShown(const EExplanation InExplanationName) const { return ShownExplanations.Contains(InExplanationName); }

bool USaveSlot::UnlockAbility(const ECharacterType InCharacter, const int32 InAbilityIndex)
{
	if (GetSkillPoints() <= 0) return false;
	FCharacterAbilities& CharacterAbilities = CharacterUnlockedAbilities.FindOrAdd(InCharacter);
	if (!CharacterAbilities.Abilities.Contains(InAbilityIndex)) CharacterAbilities.Abilities.Add(InAbilityIndex);
	SaveSlot();
	return true;
}

int32 USaveSlot::GetCurrentLevelNeededXp() const { return PlayerLevelData->GetXpNeeded(CurrentLevel); }

int32 USaveSlot::GetLevelDamageIncrease(const int32 InLevel) const { return PlayerLevelData->GetDamageIncrease(InLevel == -1 ? CurrentLevel : InLevel); }

int32 USaveSlot::GetLevelHpIncrease(const int32 InLevel) const { return PlayerLevelData->GetHpIncrease(InLevel == -1 ? CurrentLevel : InLevel); }

int32 USaveSlot::GetVariationUnlockProgress(const EVariation InVariationUnlockName) const { return VariationUnlockData.GetVariationUnlockProgress(InVariationUnlockName); }

int32 USaveSlot::GetVariationUnlockRequirement(const EVariation InVariationUnlockName) const { return VariationUnlockData.GetVariationUnlockRequirement(InVariationUnlockName); }

int32 USaveSlot::GetSkillPoints() const { return GetPlayerLevel() - CharacterUnlockedAbilities.Num(); }

EVariation USaveSlot::GetSelectedVariation(const ECharacterType InCharacter) const { return VariationUnlockData.GetSelectedVariation(InCharacter); }

FCharacterAbilities USaveSlot::GetCharacterChosenAbilities(const ECharacterType InCharacter)
{
	if (const FCharacterAbilities* CharacterAbilities = CharacterChosenAbilities.Find(InCharacter)) return *CharacterAbilities;
	return FCharacterAbilities();
}

FCharacterAbilities USaveSlot::GetCharacterUnlockedAbilities(const ECharacterType InCharacter)
{
	if (const FCharacterAbilities* CharacterAbilities = CharacterUnlockedAbilities.Find(InCharacter)) return *CharacterAbilities;
	return FCharacterAbilities();
}

void USaveSlot::AddCurrencyAmount(const int32 InCurrencyAmount)
{
	CurrencyAmount += InCurrencyAmount;
	SaveSlot();
}

void USaveSlot::SetTutorialFinished()
{
	bTutorialFinished = true;
	SaveSlot();
}

void USaveSlot::SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const FString& InSlotName)
{
	PlayerLevelData = InGameInstance->GetPlayerLevelData();
	SlotName = InSlotName;
	InGameInstance->GetSubsystem<UDelegateManager>()->OnCoinCollected.BindUObject(this, &USaveSlot::AddCurrencyAmount);
	InGameInstance->GetSubsystem<UDelegateManager>()->OnVariationSelection.AddUObject(this, &USaveSlot::UpdateSelectedVariation);
	InGameInstance->GetSubsystem<UDelegateManager>()->OnMainTutorialEnd.AddUObject(this, &USaveSlot::SetTutorialFinished);
	#if WITH_EDITOR
		CurrentLevel = InGameInstance->GetPlayerLevel();
	#endif
	VersionMigration();
}

void USaveSlot::AddXp(const TObjectPtr<UGameInstance>& InGameInstance, const int32 InXp)
{
	CurrentXp += InXp;
	while (CurrentXp >= GetCurrentLevelNeededXp())
	{
		InGameInstance->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::LevelUp);
		CurrentXp -= GetCurrentLevelNeededXp();
		InGameInstance->GetSubsystem<UDelegateManager>()->OnLevelUp.Broadcast(++CurrentLevel);
	}
}

void USaveSlot::ProgressVariationUnlock(const EVariation InVariationUnlockName, const int32 InAmount) { VariationUnlockData.ProgressVariationUnlock(InVariationUnlockName, InAmount); }

void USaveSlot::SaveSlot() { UGameplayStatics::SaveGameToSlot(this, SlotName, 0); }

void USaveSlot::SetLaunchedOnce()
{
	bIsFirstLaunch = false;
	SaveSlot();
}

void USaveSlot::UnlockVariation(const EVariation InVariationUnlockName) { VariationUnlockData.UnlockVariation(InVariationUnlockName); }

void USaveSlot::AddChosenAbilities(const ECharacterType InCharacter, const int32 InFirstAbilityIndex, const int32 InSecondAbilityIndex)
{
	FCharacterAbilities& CharacterAbilities = CharacterChosenAbilities.FindOrAdd(InCharacter);
	CharacterAbilities.Abilities.Empty();
	CharacterAbilities.Abilities.Add(InFirstAbilityIndex);
	CharacterAbilities.Abilities.Add(InSecondAbilityIndex);
}

void USaveSlot::AddShownExplanation(const EExplanation InExplanationName)
{
	ShownExplanations.Add(InExplanationName);
	if (bTutorialFinished) SaveSlot();
}

void USaveSlot::UpdateSelectedVariation(const ECharacterType InCharacter, const EVariation InVariation) { VariationUnlockData.UpdateSelectedVariation(InCharacter, InVariation); }

void USaveSlot::VersionMigration()
{
	if (SaveVersion >= CURRENT_SAVE_VERSION) return;
	SaveVersion = CURRENT_SAVE_VERSION;
	SaveSlot();
}
