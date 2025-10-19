#include "SaveSlot.h"
#include "MetaSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/PlayerLevelData.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

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

void USaveSlot::SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const FString& InSlotName)
{
	GameInstance = InGameInstance;
	PlayerLevelData = GameInstance->GetPlayerLevelData();
	SlotName = InSlotName;
	AddDefaultAbilities();
	GameInstance->GetSubsystem<UDelegateManager>()->OnVariationSelection.AddUObject(this, &USaveSlot::UpdateSelectedVariation);
	#if WITH_EDITOR
		if (GameInstance->GetUseCustomLevel()) CurrentLevel = GameInstance->GetPlayerLevel();
	#endif
	VersionMigration();
}

void USaveSlot::AddXp(const int32 InXp)
{
	CurrentXp += InXp;
	while (CurrentXp >= GetCurrentLevelNeededXp())
	{
		GameInstance->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::LevelUp);
		CurrentXp -= GetCurrentLevelNeededXp();
		GameInstance->GetSubsystem<UDelegateManager>()->OnLevelUp.Broadcast(++CurrentLevel);
	}
}

void USaveSlot::ProgressVariationUnlock(const EVariation InVariationUnlockName, const int32 InAmount) { VariationUnlockData.ProgressVariationUnlock(InVariationUnlockName, InAmount); }

void USaveSlot::SaveSlot()
{
	const FDateTime Now = FDateTime::Now();
	GameInstance->GetSubsystem<USaveSlotManager>()->GetMetaSaveSlot()->UpdateSaveSlotMetaData(SlotName, FSaveSlotMetaData(CurrentLevel, SlotName, FDateTime(Now.GetYear(), Now.GetMonth(), Now.GetDay())));
	UGameplayStatics::SaveGameToSlot(this, SlotName, 0);
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

void USaveSlot::AddArenaStats(const int32 InSurviveTime)
{
	if (InSurviveTime > ArenaMaxTimeSurvivedSeconds) ArenaMaxTimeSurvivedSeconds = InSurviveTime;
	SaveSlot();
}

void USaveSlot::UpdateSelectedVariation(const ECharacterType InCharacter, const EVariation InVariation) { VariationUnlockData.UpdateSelectedVariation(InCharacter, InVariation); }

void USaveSlot::VersionMigration()
{
	if (SaveVersion >= CURRENT_SAVE_VERSION) return;
	while (SaveVersion < CURRENT_SAVE_VERSION)
	{
		switch (SaveVersion)
		{
			case 1:
				if (UGameplayStatics::DoesSaveGameExist(TEXT("DemoPlayerSaveSlot"), 0)) UGameplayStatics::DeleteGameInSlot(TEXT("DemoPlayerSaveSlot"), 0);
				SaveVersion = 2;
				break;
			default:
				SaveVersion = CURRENT_SAVE_VERSION;
		}
	}
	SaveSlot();
}

void USaveSlot::AddDefaultAbilities()
{
	if (GetCharacterChosenAbilities(ECharacterType::Ranger).Abilities.IsEmpty()) AddChosenAbilities(ECharacterType::Ranger, 0, -1);
	if (GetCharacterChosenAbilities(ECharacterType::Guardian).Abilities.IsEmpty()) AddChosenAbilities(ECharacterType::Guardian, 0, -1);
	if (GetCharacterChosenAbilities(ECharacterType::Trickster).Abilities.IsEmpty()) AddChosenAbilities(ECharacterType::Trickster, 0, -1);
	if (GetCharacterChosenAbilities(ECharacterType::Breacher).Abilities.IsEmpty()) AddChosenAbilities(ECharacterType::Breacher, 0, -1);
	if (GetCharacterChosenAbilities(ECharacterType::Tank).Abilities.IsEmpty()) AddChosenAbilities(ECharacterType::Tank, 0, -1);
}
