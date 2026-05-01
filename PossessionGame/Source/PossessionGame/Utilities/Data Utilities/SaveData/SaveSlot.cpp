#include "SaveSlot.h"
#include "MetaSaveSlot.h"
#include "RunSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerLevelData.h"
#include "PossessionGame/Enums/EPlayerLevelUpgradeId.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
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

bool USaveSlot::IsPlayerLevelUpgradeSelected(const EPlayerLevelUpgradeId InId) const { return SelectedPlayerLevelUpgrades.Contains(InId); }

bool USaveSlot::UnlockAbility(const ECharacterType InCharacter, const int32 InAbilityIndex)
{
	if (GetSkillPoints() <= 0) return false;
	FCharacterAbilities& CharacterAbilities = CharacterUnlockedAbilities.FindOrAdd(InCharacter);
	if (!CharacterAbilities.Abilities.Contains(InAbilityIndex)) CharacterAbilities.Abilities.Add(InAbilityIndex);
	SaveSlot();
	return true;
}

int32 USaveSlot::GetVariationUnlockProgress(const EVariation InVariationUnlockName) const { return VariationUnlockData.GetVariationUnlockProgress(InVariationUnlockName); }

int32 USaveSlot::GetVariationUnlockRequirement(const EVariation InVariationUnlockName) const { return VariationUnlockData.GetVariationUnlockRequirement(InVariationUnlockName); }

int32 USaveSlot::GetSkillPoints() const { return GetPlayerLevel() - CharacterUnlockedAbilities.Num(); }

float USaveSlot::GetCurrentLevelNeededXp() const { return PlayerLevelData->GetXpNeeded(CurrentLevel); }

float USaveSlot::GetPlayerLevelPureGlobalDamageBonus() const
{
	float DamageBonus = IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FirstOptionA) ? 0.15f : 0.f;
	DamageBonus += IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::SixthOptionB) ? 0.3f : 0.f;
	return DamageBonus;
}

float USaveSlot::GetPlayerLevelAllGlobalDamageBonus() const
{
	const float SecretBoostDamage = GameInstance ? GameInstance->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardDamage) * 0.01f : 0.f;
	if (!IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FourthOptionA)) return GetPlayerLevelPureGlobalDamageBonus() + SecretBoostDamage;
	return GetPlayerLevelPureGlobalDamageBonus() + GameInstance->GetSubsystem<UPlayerDataManager>()->GetRecentHijackBoost() + SecretBoostDamage;
}

float USaveSlot::GetPlayerLevelAllGlobalHpBonus(const float InMaxHp) const
{
	float HpBonusPercentage = IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FirstOptionB) ? 25.f : 0.f;
	HpBonusPercentage += IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::SixthOptionA) ? 50.f : 0.f;
	const float HpBonus = IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::ThirdOptionB) ? 50.f : 0.f;
	return HpBonus + InMaxHp * HpBonusPercentage * 0.01f;
}

float USaveSlot::GetPlayerLevelAllCriticalMultiBonus() const
{
	float CriticalMulti = IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::ThirdOptionA) ? 0.2f : 0.f;
	CriticalMulti += IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::EighthOptionB) ? 0.4f : 0.f;
	return CriticalMulti;
}

EVariation USaveSlot::GetSelectedVariation(const ECharacterType InCharacter) const { return VariationUnlockData.GetSelectedVariation(InCharacter); }

ECharacterType USaveSlot::GetRandomSpawnableCharacter() const
{
	const TArray<ECharacterType> SpawnableCharactersArray = GameInstance->GetSpawnCharactersFromSaveEnabled() ? SpawnableCharacters.Array() : GameInstance->GetSpawnableCharacters();
	TMap<ECharacterType, int32> Weights = GameInstance->GetSpawnableCharactersWeightsToSpawn();
	if (Weights.IsEmpty() || SpawnableCharactersArray.IsEmpty()) return ECharacterType::Guardian;
	int32 TotalWeight = 0;
	for (const ECharacterType Type : SpawnableCharactersArray)
	{
		if (const int32* Weight = Weights.Find(Type)) TotalWeight += *Weight;
	}
	int32 Roll = FMath::RandRange(1, TotalWeight);
	for (const ECharacterType Type : SpawnableCharactersArray)
	{
		if (const int32* Weight = Weights.Find(Type))
		{
			Roll -= *Weight;
			if (Roll <= 0) return Type;
		}
	}
	return ECharacterType::Guardian;
}

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

void USaveSlot::AddSpawnableCharacter(const TObjectPtr<UDelegateManager>& InDelegateManager, const ECharacterType InCharacterName)
{
	SpawnableCharacters.Add(InCharacterName);
	(void)InDelegateManager->RequestAddAbilitiesToUpgradePool.ExecuteIfBound(this);
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

void USaveSlot::AddXp(const float InXp)
{
	if (GameInstance->GetSubsystem<UDelegateManager>()->RequestIsArenaMode.IsBound() && GameInstance->GetSubsystem<UDelegateManager>()->RequestIsArenaMode.Execute()) return;
	CurrentXp += InXp;
	while (CurrentXp >= GetCurrentLevelNeededXp())
	{
		GameInstance->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::LevelUp);
		CurrentXp -= GetCurrentLevelNeededXp();
		CurrentLevel++;
		GameInstance->GetSubsystem<UDelegateManager>()->OnLevelUp.Broadcast();
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

void USaveSlot::ChangePlayerLevelUpgrade(const EPlayerLevelUpgradeId InAddId, const EPlayerLevelUpgradeId InRemoveId)
{
	SelectedPlayerLevelUpgrades.Remove(InRemoveId);
	SelectedPlayerLevelUpgrades.Add(InAddId);
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
	if (GetCharacterChosenAbilities(ECharacterType::Wraith).Abilities.IsEmpty()) AddChosenAbilities(ECharacterType::Wraith, 0, -1);
	if (GetCharacterChosenAbilities(ECharacterType::Spy).Abilities.IsEmpty()) AddChosenAbilities(ECharacterType::Spy, 0, -1);
	if (GetCharacterChosenAbilities(ECharacterType::Samurai).Abilities.IsEmpty()) AddChosenAbilities(ECharacterType::Samurai, 0, -1);
}
