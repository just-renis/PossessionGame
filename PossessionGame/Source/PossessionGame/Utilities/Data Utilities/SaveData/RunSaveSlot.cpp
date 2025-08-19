#include "RunSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

float URunSaveSlot::GetUpgradeAttributeValues(const EUpgrade InUpgradeName, const EUpgradeAttribute InUpgradeAttribute, const bool bIsPlayer)
{
	UE_LOG(LogTemp, Error, TEXT("URunSaveSlot::GetUpgradeAttributeValues0 %s %s"), *UEnum::GetValueAsString(InUpgradeName), *UEnum::GetValueAsString(InUpgradeAttribute));
	if (const FUpgradeAttributeValues* AttributeValues = UnlockedUpgrades.Find(InUpgradeName))
	{
		UE_LOG(LogTemp, Error, TEXT("URunSaveSlot::GetUpgradeAttributeValues1 %s %s"), *UEnum::GetValueAsString(InUpgradeName), *UEnum::GetValueAsString(InUpgradeAttribute));
		TMap<EUpgradeAttribute, float> UpgradeAttributesValues = bIsPlayer ? AttributeValues->PlayerUpgrades : AttributeValues->EnemyUpgrades;
		if (const float* Value = UpgradeAttributesValues.Find(InUpgradeAttribute))
		{
			UE_LOG(LogTemp, Error, TEXT("URunSaveSlot::GetUpgradeAttributeValues2 %f"), *Value);
			return *Value;
		}
	}
	return 0.f;
}

int32 URunSaveSlot::GetSecretBoost(const EVariation InVariation)
{
	if (const int32* BoostAmount = SecretCrateBoosts.Find(InVariation)) return *BoostAmount;
	return 0;
}

int32 URunSaveSlot::GetCurrentLevel(const EUpgrade InUpgradeName)
{
	if (const FUpgradeAttributeValues* UpgradeAttributeValues = UnlockedUpgrades.Find(InUpgradeName)) return UpgradeAttributeValues->CurrentLevel;
	return 0;
}

TSet<int32> URunSaveSlot::GetUpgradeChoicesIds(const EUpgrade InUpgradeName)
{
	if (const FUpgradeChoicesIds* UpgradeChoicesIds = ChosenUpgradeChoices.Find(InUpgradeName)) return UpgradeChoicesIds->Ids;
	return TSet<int32>();
}

void URunSaveSlot::AddUpgrade(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const EUpgrade InUpgrade, const FUpgradeChoicePair& InUpgradeChoicePair)
{
	FUpgradeAttributeValues& UpgradeAttributeValues = UnlockedUpgrades.FindOrAdd(InUpgrade);
	UpgradeAttributeValues.AddUpgrade(InUpgradeChoicePair);
	FUpgradeChoicesIds& UpgradeChoicesIds = ChosenUpgradeChoices.FindOrAdd(InUpgrade);
	UpgradeChoicesIds.Ids.Add(InUpgradeChoicePair.Id);
	(void)InGameInstance->GetSubsystem<UDelegateManager>()->ApplyUpgradeOnSelection.ExecuteIfBound(GetCharacterNameByUpgrade(InUpgrade));
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

ECharacterType URunSaveSlot::GetCharacterNameByUpgrade(const EUpgrade InUpgradeName)
{
	switch (InUpgradeName)
	{
		case EUpgrade::GuardianAbility1: case EUpgrade::GuardianAbility2: return ECharacterType::Guardian;
		case EUpgrade::RangerAbility1: case EUpgrade::RangerAbility2: return ECharacterType::Ranger;
		case EUpgrade::TricksterAbility1: case EUpgrade::TricksterAbility2: return ECharacterType::Trickster;
	}
	return ECharacterType::MinionBoss;
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
