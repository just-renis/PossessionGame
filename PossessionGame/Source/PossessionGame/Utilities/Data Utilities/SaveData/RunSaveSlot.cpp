#include "RunSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EAbility.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FSpecialUpgradesContainer.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

bool URunSaveSlot::IsSpecialUpgradeAvailable(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialUpgradeAttribute)
{
	if (const FSpecialUpgradesContainer* SpecialUpgradesContainerData = SpecialUpgrades.Find(InUpgradeName))
	{
		if (SpecialUpgradesContainerData->Contains(InSpecialUpgradeAttribute)) return true;
	}
	return false;
}

float URunSaveSlot::GetUpgradeAttributeValues(const EUpgrade InUpgradeName, const EUpgradeAttribute InUpgradeAttribute, const bool bIsPlayer)
{
	if (const FUpgradeAttributeValues* AttributeValues = UnlockedUpgrades.Find(InUpgradeName))
	{
		TMap<EUpgradeAttribute, float> UpgradeAttributesValues = bIsPlayer ? AttributeValues->PlayerUpgrades : AttributeValues->EnemyUpgrades;
		if (const float* Value = UpgradeAttributesValues.Find(InUpgradeAttribute)) return *Value;
	}
	return 0.f;
}

float URunSaveSlot::GetUpgradeAttributeValues(const EAbility InAbilityName, const EUpgradeAttribute InUpgradeAttribute, const bool bIsPlayer)
{
	if (const FUpgradeAttributeValues* AttributeValues = UnlockedUpgrades.Find(GetUpgradeByAbilityName(InAbilityName)))
	{
		TMap<EUpgradeAttribute, float> UpgradeAttributesValues = bIsPlayer ? AttributeValues->PlayerUpgrades : AttributeValues->EnemyUpgrades;
		if (const float* Value = UpgradeAttributesValues.Find(InUpgradeAttribute)) return *Value;
	}
	return 0.f;
}

float URunSaveSlot::GetUpgradeAttributeValues(const EPoolObject InPoolObject, const EUpgradeAttribute InUpgradeAttribute, const bool bIsPlayer, const bool bForAbilityOnly)
{
	if (!bIsPlayer) return GetUpgradeAttributeValuesForEnemy(InPoolObject, InUpgradeAttribute, bForAbilityOnly);
	if (const FUpgradeAttributeValues* AttributeValues = UnlockedUpgrades.Find(GetUpgradeByPoolObjectName(InPoolObject)))
	{
		TMap<EUpgradeAttribute, float> UpgradeAttributesValues = bIsPlayer ? AttributeValues->PlayerUpgrades : AttributeValues->EnemyUpgrades;
		if (const float* Value = UpgradeAttributesValues.Find(InUpgradeAttribute)) return *Value;
	}
	return 0.f;
}

float URunSaveSlot::GetHpUpgradesForEnemy(const ECharacterType InCharacterType)
{
	TArray<EUpgrade> Upgrades = GetHpUpgradesByCharacterName(InCharacterType);
	float Value = 0.f;
	for (int32 i = 0; i < Upgrades.Num(); i++)
	{
		if (const FUpgradeAttributeValues* AttributeValues = UnlockedUpgrades.Find(Upgrades[i]))
		{
			TMap<EUpgradeAttribute, float> UpgradeAttributesValues = AttributeValues->EnemyUpgrades;
			if (const float* FoundValue = UpgradeAttributesValues.Find(EUpgradeAttribute::FlatHealth)) Value += *FoundValue;
		}
	}
	return Value;
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

int32 URunSaveSlot::GetCurrentLevelSumOfCharacterAbilities(const ECharacterType InCharacterName)
{
	switch (InCharacterName)
	{
		case ECharacterType::Ranger: return GetCurrentLevel(EUpgrade::RangerAbility1) + GetCurrentLevel(EUpgrade::RangerAbility2) + GetCurrentLevel(EUpgrade::RangerAbility3);
		case ECharacterType::Trickster: return GetCurrentLevel(EUpgrade::TricksterAbility1) + GetCurrentLevel(EUpgrade::TricksterAbility2) + GetCurrentLevel(EUpgrade::TricksterAbility3);
		case ECharacterType::Guardian: return GetCurrentLevel(EUpgrade::GuardianAbility1) + GetCurrentLevel(EUpgrade::GuardianAbility2) + GetCurrentLevel(EUpgrade::GuardianAbility3);
		case ECharacterType::Breacher: return GetCurrentLevel(EUpgrade::BreacherAbility1) + GetCurrentLevel(EUpgrade::BreacherAbility2) + GetCurrentLevel(EUpgrade::BreacherAbility3);
		default: ;
	}
	return 0;
}

float URunSaveSlot::GetCurrentUpgradeXpNeeded() const { return 10 + 5 * CurrentUpgradeLevel; }

TSet<int32> URunSaveSlot::GetUpgradeChoicesIds(const EUpgrade InUpgradeName)
{
	if (const FUpgradeChoicesIds* UpgradeChoicesIds = ChosenUpgradeChoices.Find(InUpgradeName)) return UpgradeChoicesIds->Ids;
	return TSet<int32>();
}

void URunSaveSlot::AddUpgrade(const TObjectPtr<UGameInstance>& InGameInstance, const EUpgrade InUpgrade, const FUpgradeChoicePair& InUpgradeChoicePair, const EUpgradeAttribute InSpecialAttribute)
{
	FUpgradeAttributeValues& UpgradeAttributeValues = UnlockedUpgrades.FindOrAdd(InUpgrade);
	UpgradeAttributeValues.AddUpgrade(InUpgradeChoicePair);
	FUpgradeChoicesIds& UpgradeChoicesIds = ChosenUpgradeChoices.FindOrAdd(InUpgrade);
	UpgradeChoicesIds.Ids.Add(InUpgradeChoicePair.Id);
	if (InSpecialAttribute != EUpgradeAttribute::None)
	{
		FSpecialUpgradesContainer& SpecialUpgradesContainer = SpecialUpgrades.FindOrAdd(InUpgrade);
		SpecialUpgradesContainer.Add(InSpecialAttribute);
		InGameInstance->GetSubsystem<UDelegateManager>()->OnSpecialUpgradeAdded.Broadcast(InUpgrade, InSpecialAttribute);
	}
	(void)InGameInstance->GetSubsystem<UDelegateManager>()->ApplyUpgradeOnSelection.ExecuteIfBound();
}

void URunSaveSlot::SaveSlot() { UGameplayStatics::SaveGameToSlot(this, SlotName, 0); }

void URunSaveSlot::SetupSlot(const TObjectPtr<UGameInstance>& InGameInstance, const FString& InSlotName)
{
	InGameInstance->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.BindUObject(this, &URunSaveSlot::IsSpecialUpgradeAvailable);
	SlotName = InSlotName;
	VersionMigration();
}

void URunSaveSlot::AddSecretCrateBoost(const EVariation InVariation, const int32 InPercentage)
{
	int32& BoostAmount = SecretCrateBoosts.FindOrAdd(InVariation);
	BoostAmount += InPercentage;
}

void URunSaveSlot::SetCurrentChapterAndCharacter(const int32 InCurrentChapter, const ECharacterType InCurrentCharacter)
{
	CurrentChapter = InCurrentChapter > 4 ? 4 : InCurrentChapter;
	CurrentCharacter = InCurrentCharacter;
	SaveSlot();
}

void URunSaveSlot::AddUpgradeXp(const TObjectPtr<UPossessionGameInstance>& InGameInstance, const float InUpgradeXp)
{
	CurrentUpgradeXp += InUpgradeXp;
	while (CurrentUpgradeXp >= GetCurrentUpgradeXpNeeded())
	{
		InGameInstance->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeAvailable);
		CurrentUpgradeXp -= GetCurrentUpgradeXpNeeded();
		CurrentUpgradeLevel++;
		InGameInstance->GetSubsystem<UDelegateManager>()->OnUpgradeAmountChanged.ExecuteIfBound(++CurrentlyAvailableUpgradeAmount);
	}
}

void URunSaveSlot::SubtractCurrentUpgradeAmount(const TObjectPtr<UDelegateManager>& InDelegateManager)
{
	InDelegateManager->OnUpgradeAmountChanged.ExecuteIfBound(--CurrentlyAvailableUpgradeAmount);
}

float URunSaveSlot::GetUpgradeAttributeValuesForEnemy(const EPoolObject InPoolObject, const EUpgradeAttribute InUpgradeAttribute, const bool bForAbilityOnly)
{
	TArray<EUpgrade> Upgrades = GetUpgradesByPoolObjectName(InPoolObject, bForAbilityOnly);
	float Value = 0.f;
	for (int32 i = 0; i < Upgrades.Num(); i++)
	{
		if (const FUpgradeAttributeValues* AttributeValues = UnlockedUpgrades.Find(Upgrades[i]))
		{
			TMap<EUpgradeAttribute, float> UpgradeAttributesValues = AttributeValues->EnemyUpgrades;
			if (const float* FoundValue = UpgradeAttributesValues.Find(InUpgradeAttribute)) Value += *FoundValue;
		}
	}
	return Value;
}

EUpgrade URunSaveSlot::GetUpgradeByAbilityName(const EAbility InAbilityName)
{
	switch (InAbilityName)
	{
		case EAbility::GuardianAbility1: return EUpgrade::GuardianAbility1;
		case EAbility::GuardianAbility2: return EUpgrade::GuardianAbility2;
		case EAbility::GuardianAbility3: return EUpgrade::GuardianAbility3;
		case EAbility::RangerAbility1: return EUpgrade::RangerAbility1;
		case EAbility::RangerAbility2: return EUpgrade::RangerAbility2;
		case EAbility::RangerAbility3: return EUpgrade::RangerAbility3;
		case EAbility::TricksterAbility1: return EUpgrade::TricksterAbility1;
		case EAbility::TricksterAbility2: return EUpgrade::TricksterAbility2;
		case EAbility::TricksterAbility3: return EUpgrade::TricksterAbility3;
		case EAbility::BreacherAbility1: return EUpgrade::BreacherAbility1;
		case EAbility::BreacherAbility2: return EUpgrade::BreacherAbility2;
		case EAbility::BreacherAbility3: return EUpgrade::BreacherAbility3;
		case EAbility::WraithAbility1: return EUpgrade::WraithAbility1;
		case EAbility::WraithAbility2: return EUpgrade::WraithAbility2;
		case EAbility::WraithAbility3: return EUpgrade::WraithAbility3;
		case EAbility::SpyAbility1: return EUpgrade::SpyAbility1;
		case EAbility::SpyAbility2: return EUpgrade::SpyAbility2;
		case EAbility::SpyAbility3: return EUpgrade::SpyAbility3;
		case EAbility::TankAbility1: return EUpgrade::TankAbility1;
		case EAbility::TankAbility2: return EUpgrade::TankAbility2;
		case EAbility::TankAbility3: return EUpgrade::TankAbility3;
		default: ;
	}
	return EUpgrade::None;
}

EUpgrade URunSaveSlot::GetUpgradeByPoolObjectName(const EPoolObject InPoolObjectName)
{
	switch (InPoolObjectName)
	{
		case EPoolObject::RangerGrenadeLauncherProjectile: return EUpgrade::RangerAbility1;
		case EPoolObject::TricksterPoisonProjectile: return EUpgrade::TricksterAbility1;
		case EPoolObject::TricksterEmpProjectile: return EUpgrade::TricksterAbility2;
		case EPoolObject::CloneProjectile: return EUpgrade::TricksterAbility3;
		case EPoolObject::BreacherBarrel: return EUpgrade::BreacherAbility1;
		case EPoolObject::BreacherDynamite: return EUpgrade::BreacherAbility3;
		case EPoolObject::WraithMolotov: return EUpgrade::WraithAbility1;
		default: ;
	}
	return EUpgrade::None;
}

TArray<EUpgrade> URunSaveSlot::GetUpgradesByPoolObjectName(const EPoolObject InPoolObjectName, const bool bForAbilityOnly)
{
	if (bForAbilityOnly)
	{
		switch (InPoolObjectName)
		{
			case EPoolObject::RangerGrenadeLauncherProjectile: return TArray({ EUpgrade::RangerAbility1, EUpgrade::RangerAbility2, EUpgrade::RangerAbility3 });
			case EPoolObject::TricksterEmpProjectile: case EPoolObject::TricksterPoisonProjectile: return TArray({ EUpgrade::TricksterAbility1, EUpgrade::TricksterAbility2, EUpgrade::TricksterAbility3 });
			case EPoolObject::BreacherBarrel: case EPoolObject::BreacherDynamite: return TArray({ EUpgrade::BreacherAbility1, EUpgrade::BreacherAbility2, EUpgrade::BreacherAbility3 });
			case EPoolObject::WraithMolotov: return TArray({ EUpgrade::WraithAbility1, EUpgrade::WraithAbility2, EUpgrade::WraithAbility3 });
			default: ;
		}
	}
	else
	{
		switch (InPoolObjectName)
		{
			case EPoolObject::GuardianProjectile: return TArray({ EUpgrade::GuardianAbility1, EUpgrade::GuardianAbility2, EUpgrade::GuardianAbility3 });
			case EPoolObject::RangerProjectile: case EPoolObject::RangerGrenadeLauncherProjectile: return TArray({ EUpgrade::RangerAbility1, EUpgrade::RangerAbility2, EUpgrade::RangerAbility3 });
			case EPoolObject::TricksterProjectile: return TArray({ EUpgrade::TricksterAbility1, EUpgrade::TricksterAbility2, EUpgrade::TricksterAbility3 });
			case EPoolObject::BreacherProjectile: return TArray({ EUpgrade::BreacherAbility1, EUpgrade::BreacherAbility2, EUpgrade::BreacherAbility3 });
			case EPoolObject::WraithProjectile: return TArray({ EUpgrade::WraithAbility1, EUpgrade::WraithAbility2, EUpgrade::WraithAbility3 });
			case EPoolObject::SpyProjectile: return TArray({ EUpgrade::SpyAbility1, EUpgrade::SpyAbility2, EUpgrade::SpyAbility3 });
			case EPoolObject::TankProjectile: return TArray({ EUpgrade::TankAbility1, EUpgrade::TankAbility2, EUpgrade::TankAbility3 });
			default: ;
		}
	}
	return {};
}

TArray<EUpgrade> URunSaveSlot::GetHpUpgradesByCharacterName(const ECharacterType InCharacterName)
{
	switch (InCharacterName)
	{
		case ECharacterType::Guardian: return TArray({ EUpgrade::GuardianAbility1, EUpgrade::GuardianAbility2, EUpgrade::GuardianAbility3 });
		case ECharacterType::Trickster: return TArray({ EUpgrade::TricksterAbility1, EUpgrade::TricksterAbility2, EUpgrade::TricksterAbility3 });
		case ECharacterType::Ranger: return TArray({ EUpgrade::RangerAbility1, EUpgrade::RangerAbility2, EUpgrade::RangerAbility3 });
		case ECharacterType::Breacher: return TArray({ EUpgrade::BreacherAbility1, EUpgrade::BreacherAbility2, EUpgrade::BreacherAbility3 });
		case ECharacterType::Wraith: return TArray({ EUpgrade::WraithAbility1, EUpgrade::WraithAbility2, EUpgrade::WraithAbility3 });
		case ECharacterType::Spy: return TArray({ EUpgrade::SpyAbility1, EUpgrade::SpyAbility2, EUpgrade::SpyAbility3 });
		default: ;
	}
	return {};
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
