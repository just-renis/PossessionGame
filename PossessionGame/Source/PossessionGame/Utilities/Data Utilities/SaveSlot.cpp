#include "SaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/PlayerLevelData.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

constexpr int32 CURRENT_SAVE_VERSION = 1;

bool USaveSlot::GetIsVariationUnlocked(const EVariationUnlock InVariationUnlockName) const { return VariationUnlockData.IsVariationUnlocked(InVariationUnlockName); }

int32 USaveSlot::GetCurrentLevelNeededXp() const { return PlayerLevelData->GetXpNeeded(CurrentLevel); }

int32 USaveSlot::GetLevelDamageIncrease(const int32 InLevel) const { return PlayerLevelData->GetDamageIncrease(InLevel == -1 ? CurrentLevel : InLevel); }

int32 USaveSlot::GetLevelHpIncrease(const int32 InLevel) const { return PlayerLevelData->GetHpIncrease(InLevel == -1 ? CurrentLevel : InLevel); }

int32 USaveSlot::GetVariationUnlockProgress(const EVariationUnlock InVariationUnlockName) const { return VariationUnlockData.GetVariationUnlockProgress(InVariationUnlockName); }

int32 USaveSlot::GetVariationUnlockRequirement(const EVariationUnlock InVariationUnlockName) const { return VariationUnlockData.GetVariationUnlockRequirement(InVariationUnlockName); }

EVariation USaveSlot::GetSelectedVariation(const ECharacterType InCharacter) const { return VariationUnlockData.GetSelectedVariation(InCharacter); }

void USaveSlot::SetTutorialFinished()
{
	bTutorialFinished = true;
	SaveSlot();
}

void USaveSlot::SetupSlot(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const FString& InSlotName)
{
	PlayerLevelData = InGameInstance->GetPlayerLevelData();
	SlotName = InSlotName;
	InGameInstance->GetSubsystem<UDelegateManager>()->OnVariationSelection.AddUObject(this, &USaveSlot::UpdateSelectedVariation);
	InGameInstance->GetSubsystem<UDelegateManager>()->OnMainTutorialEnd.AddUObject(this, &USaveSlot::SetTutorialFinished);
	#if WITH_EDITOR
		CurrentLevel = InGameInstance->GetPlayerLevel();
	#endif
	VersionMigration();
}

void USaveSlot::AddXp(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const int32 InXp)
{
	CurrentXp += InXp;
	while (CurrentXp >= GetCurrentLevelNeededXp())
	{
		InGameInstance->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::LevelUp);
		CurrentXp -= GetCurrentLevelNeededXp();
		InGameInstance->GetSubsystem<UDelegateManager>()->OnLevelUp.Broadcast(++CurrentLevel);
	}
}

void USaveSlot::ProgressVariationUnlock(const EVariationUnlock InVariationUnlockName) { VariationUnlockData.ProgressVariationUnlock(InVariationUnlockName); }

void USaveSlot::SaveSlot() { UGameplayStatics::SaveGameToSlot(this, SlotName, 0); }

void USaveSlot::SetLaunchedOnce()
{
	bIsFirstLaunch = false;
	SaveSlot();
}

void USaveSlot::UpdateSelectedVariation(const ECharacterType InCharacter, const EVariation InVariation) { VariationUnlockData.UpdateSelectedVariation(InCharacter, InVariation); }

void USaveSlot::VersionMigration()
{
	if (SaveVersion >= CURRENT_SAVE_VERSION) return;
	SaveVersion = CURRENT_SAVE_VERSION;
	SaveSlot();
}
