#include "PossessionGameGameModeBase.h"
#include "PossessionGameInstance.h"
#include "SettingsSave.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

TArray<TSubclassOf<APrefab>>* APossessionGameGameModeBase::GetPrefabsByCategory(const EPrefabCategory Category)
{
	if (Prefabs.Contains(Category)) return &Prefabs[Category].PrefabClasses;
	return nullptr;
}

void APossessionGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	LoadSettingsData();
	GetWorld()->GetSubsystem<UWidgetManager>()->Init(WidgetClasses);
	GetWorld()->GetSubsystem<UUpgradeManager>()->Init(Upgrades);
	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (!LevelName.Equals("TutorialLevel")) SaveSlot->SetTutorialFinished();
	const TObjectPtr<UWaveManager> WaveManager = GetWorld()->GetSubsystem<UWaveManager>();
	WaveManager->Init(SaveSlot->GetIsTutorialFinished(), DelegateManager);
	WaveManager->SetupWaveManagerSettings(EnemiesThatAreSpawnable, EnemiesAmountToSpawnOnFirstWave);
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->Init(DelegateManager, TargetEQS);
	GetWorld()->GetSubsystem<UPoolingManager>()->Init(PoolObjects);
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::ObjectiveMarker)->Init(false);
	GetWorld()->GetSubsystem<UPostProcessManager>()->Init(PostProcessMaterials);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
}

void APossessionGameGameModeBase::LoadSettingsData()
{
	const FString SettingsSlot = "SettingsSlot";
	if (UGameplayStatics::DoesSaveGameExist(SettingsSlot, 0))
	{
		const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot(SettingsSlot, 0));
		SavedSettings->LoadSettings(GetWorld());
	}
	else
	{
		const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::CreateSaveGameObject(SettingsSaveClass));
		SavedSettings->LoadSettingsOnFirstLaunch(GetWorld());
		UGameplayStatics::SaveGameToSlot(SavedSettings, SettingsSlot, 0);
	}
}
