#include "PossessionGameInstance.h"
#include "PossessionGameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/MetaSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void UPossessionGameInstance::InitGameInstanceManagers()
{
	GetTimerManager().SetTimer(TickHandle, this, &UPossessionGameInstance::Tick, 0.2f, true);
	if (!GetSubsystem<USaveSlotManager>()->GetSaveSlot()) return;
	GetSubsystem<UPrimaryDataAssetsManager>()->Init(GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished(), SpawnableEnemiesByChapter);
}

void UPossessionGameInstance::Init()
{
	Super::Init();
	GetSubsystem<UGlobalSettingsManager>()->SetCodexData(CodexData);
	GetSubsystem<USaveSlotManager>()->LoadMetaSaveSlot("MetaSaveSlot");
	GetSubsystem<UDelegateManager>()->LoadLevel.BindUObject(this, &UPossessionGameInstance::OpenLevel);
	GetSubsystem<UDelegateManager>()->RequestAddAbilitiesToUpgradePool.BindUObject(this, &UPossessionGameInstance::AddAvailableUpgrades);
	GetSubsystem<UDelegateManager>()->RequestIsDemo.BindUObject(this, &UPossessionGameInstance::IsDemo);
	const TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	if (GetSubsystem<USaveSlotManager>()->GetMetaSaveSlot()->GetIsFirstLaunch())
	{
		GameUserSettings->LoadSettings(false);
		GameUserSettings->SetFrameRateLimit(60.f);
		GameUserSettings->ApplySettings(true);
	}
	else GameUserSettings->LoadSettings(false);
}

void UPossessionGameInstance::Tick() const { GetSubsystem<UPlayerDataManager>()->Tick(TickTime); }

void UPossessionGameInstance::Shutdown()
{
	GetTimerManager().ClearTimer(TickHandle);
	Super::Shutdown();
}

void UPossessionGameInstance::OpenLevel(const FLoadLevelData InLoadLevelData) const
{
	GetSubsystem<UPlayerDataManager>()->Reset();
	if (InLoadLevelData.bSaveSlot) GetSubsystem<USaveSlotManager>()->GetSaveSlot()->SaveSlot();
	else if (InLoadLevelData.bResetSaveSlot) GetSubsystem<USaveSlotManager>()->ResetSaveSlot();
	if (InLoadLevelData.bResetRunData) GetSubsystem<USaveSlotManager>()->ResetRunSaveSlot();
	UGameplayStatics::OpenLevel(GetWorld(), InLoadLevelData.LevelName);
}

void UPossessionGameInstance::AddAvailableUpgrades(const TObjectPtr<USaveSlot>& InSaveSlot) const
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	TArray<EUpgrade> AvailableUpgrades;
	TArray AvailableCharacters = GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetAvailableCharacters();
	for (int32 i = 0; i < AvailableCharacters.Num(); i++)
	{
		const ECharacterType CharacterType = AvailableCharacters[i];
		const FCharacterUpgrades* UpgradeList = ExistingUpgrades.Find(CharacterType);
		if (UpgradeList == nullptr) continue;
		const TArray<int32>& ChosenAbilities = InSaveSlot->GetCharacterChosenAbilities(CharacterType).Abilities;
		for (int32 j = 0; j < ChosenAbilities.Num(); j++)
		{
			const int32 AbilityIndex = ChosenAbilities[j];
			if (UpgradeList->Upgrades.IsValidIndex(AbilityIndex)) AvailableUpgrades.Add(UpgradeList->Upgrades[AbilityIndex]);
		}
	}
	RunSaveSlot->AddAvailableUpgrades(AvailableUpgrades);
}
