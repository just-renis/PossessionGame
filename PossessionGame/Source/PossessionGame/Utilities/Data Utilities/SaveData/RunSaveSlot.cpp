#include "RunSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

int32 URunSaveSlot::GetLevelOfChoice(const EUpgrade InUpgrade, const int32 InChoice)
{
	if (const FUpgradeChoicesAmountByLevel* ChoicesAmountByLevel = UnlockedUpgrades.Find(InUpgrade)) return ChoicesAmountByLevel->GetLevelOfChoice(InChoice);
	return 0;
}

int32 URunSaveSlot::GetLevelOfAnUpgrade(const EUpgrade InUpgrade)
{
	if (const FUpgradeChoicesAmountByLevel* Choices = UnlockedUpgrades.Find(InUpgrade)) return Choices->GetCurrentLevelOfAnUpgrade();
	return 0;
}

int32 URunSaveSlot::GetSecretBoost(const EVariation InVariation)
{
	if (const int32* BoostAmount = SecretCrateBoosts.Find(InVariation)) return *BoostAmount;
	return 0;
}

const TMap<int32, int32>* URunSaveSlot::GetUpgradeChoicesData(const EUpgrade InUpgrade)
{
	if (const FUpgradeChoicesAmountByLevel* Choices = UnlockedUpgrades.Find(InUpgrade)) return &Choices->Data;
	return nullptr;
}

void URunSaveSlot::AddUpgrade(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const EUpgrade InUpgrade, const int32 InChoice)
{
	if (UnlockedUpgrades.Find(InUpgrade)) UnlockedUpgrades[InUpgrade].AddOrUpdate(InChoice);
	else UnlockedUpgrades.Add(InUpgrade, FUpgradeChoicesAmountByLevel(InChoice));
	(void)InGameInstance->GetSubsystem<UDelegateManager>()->ApplyUpgradeOnSelection.ExecuteIfBound(InUpgrade);
}

void URunSaveSlot::SaveSlot() { UGameplayStatics::SaveGameToSlot(this, SlotName, 0); }

void URunSaveSlot::SetupSlot(const FString& InSlotName)
{
	SlotName = InSlotName;
	VersionMigration();
}

void URunSaveSlot::AddSecretCrateBoost(const EVariation InVariation, const int32 InPercentage)
{
	int32& BoostAmount = SecretCrateBoosts.FindOrAdd(InVariation);
	BoostAmount += InPercentage;
}

void URunSaveSlot::VersionMigration()
{
	if (SaveVersion >= CURRENT_SAVE_VERSION) return;
	while (SaveVersion < CURRENT_SAVE_VERSION)
	{
		switch (SaveVersion)
		{
			case 1:
				if (UGameplayStatics::DoesSaveGameExist(TEXT("DemoPlayerRunSaveSlot"), 0)) UGameplayStatics::DeleteGameInSlot(TEXT("DemoPlayerRunSaveSlot"), 0);
				SaveVersion = 2;
				break;
			default:
				SaveVersion = CURRENT_SAVE_VERSION;
		}
	}
	SaveSlot();
}
