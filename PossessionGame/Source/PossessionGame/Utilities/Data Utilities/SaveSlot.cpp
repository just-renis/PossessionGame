#include "SaveSlot.h"
#include "PossessionGame/Data Assets/Base Data Assets/PlayerLevelData.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool USaveSlot::GetIsVariationUnlocked(const EVariationUnlock InVariationUnlockName) const { return VariationUnlockData.IsVariationUnlocked(InVariationUnlockName); }

int32 USaveSlot::GetLevelOfAnUpgrade(const EUpgrade InUpgrade) { return UnlockedUpgrades.Find(InUpgrade) ? UnlockedUpgrades[InUpgrade] : 0; }

int32 USaveSlot::GetLevelDamageIncrease(const int32 InLevel) const { return PlayerLevelData->GetDamageIncrease(InLevel == -1 ? CurrentLevel : InLevel); }

int32 USaveSlot::GetLevelHpIncrease(const int32 InLevel) const { return PlayerLevelData->GetHpIncrease(InLevel == -1 ? CurrentLevel : InLevel); }

int32 USaveSlot::GetVariationUnlockProgress(const EVariationUnlock InVariationUnlockName) const { return VariationUnlockData.GetVariationUnlockProgress(InVariationUnlockName); }

int32 USaveSlot::GetVariationUnlockRequirement(const EVariationUnlock InVariationUnlockName) const { return VariationUnlockData.GetVariationUnlockRequirement(InVariationUnlockName); }

EVariation USaveSlot::GetSelectedVariation(const ECharacterType InCharacter) const { return VariationUnlockData.GetSelectedVariation(InCharacter); }

void USaveSlot::SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance)
{
	GameInstance = InGameInstance;
	PlayerLevelData = GameInstance->GetPlayerLevelData();
	GameInstance->GetSubsystem<UDelegateManager>()->OnVariationSelection.AddUObject(this, &USaveSlot::UpdateSelectedVariation);
	#if WITH_EDITOR
		CurrentLevel = GameInstance->GetPlayerLevel();
	#endif
}

void USaveSlot::AddUpgrade(const EUpgrade InUpgrade, const int32 InLevel)
{
	if (UnlockedUpgrades.Find(InUpgrade)) UnlockedUpgrades[InUpgrade] = InLevel;
	else UnlockedUpgrades.Add(InUpgrade, InLevel);
	GameInstance->GetSubsystem<UDelegateManager>()->ApplyUpgradeOnSelection.Broadcast(InUpgrade);
}

void USaveSlot::AddXp(const int32 InXp)
{
	CurrentXp += InXp;
	if (CurrentXp >= GetCurrentLevelNeededXp())
	{
		GameInstance->GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::LevelUp);
		CurrentXp -= GetCurrentLevelNeededXp();
		GameInstance->GetSubsystem<UDelegateManager>()->OnLevelUp.Broadcast(++CurrentLevel);
	}
}

void USaveSlot::ProgressVariationUnlock(const EVariationUnlock InVariationUnlockName) { VariationUnlockData.ProgressVariationUnlock(InVariationUnlockName); }

void USaveSlot::UpdateSelectedVariation(const ECharacterType InCharacter, const EVariation InVariation) { VariationUnlockData.UpdateSelectedVariation(InCharacter, InVariation); }
