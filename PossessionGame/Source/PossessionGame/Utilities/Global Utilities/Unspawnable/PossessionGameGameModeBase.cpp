#include "PossessionGameGameModeBase.h"
#include "PossessionGameInstance.h"
#include "SettingsSave.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/ArtifactSpawnLocation.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
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
	GetWorld()->GetSubsystem<UVariationManager>()->Init(SaveSlot);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->Init(SaveSlot->GetIsTutorialFinished(), DelegateManager, EnemiesThatAreSpawnable);
	GetWorld()->GetSubsystem<UWidgetManager>()->Init(WidgetClasses);
	GetWorld()->GetSubsystem<UUpgradeManager>()->Init(Upgrades);
//	FString LevelName = GetWorld()->GetMapName();
//	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
//	if (!LevelName.Equals("TutorialLevel")) SaveSlot->SetTutorialFinished();
	GetWorld()->GetSubsystem<UWaveManager>()->Init(DelegateManager, GameModesData);
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->Init(DelegateManager, TargetEQS);
	GetWorld()->GetSubsystem<UPoolingManager>()->Init(PoolObjects);
	GetWorld()->GetSubsystem<UPostProcessManager>()->Init(PostProcessMaterials);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
	SpawnArtifact();
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

void APossessionGameGameModeBase::SpawnArtifact() const
{
	TArray<AActor*> SpawnLocations;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArtifactSpawnLocation::StaticClass(), SpawnLocations);
	if (SpawnLocations.IsEmpty()) return;
	Cast<AArtifactSpawnLocation>(SpawnLocations[FMath::RandRange(0, SpawnLocations.Num() - 1)])->SpawnArtifact();
}
