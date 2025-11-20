#include "PossessionGameGameModeBase.h"
#include "PossessionGameInstance.h"
#include "SettingsSave.h"
#include "ShaderPipelineCache.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/LoadingScreenUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/ArtifactSpawnLocation.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

TArray<TSubclassOf<APrefab>>* APossessionGameGameModeBase::GetPrefabsByCategory(const EPrefabCategory Category)
{
	if (Prefabs.Contains(Category)) return &Prefabs[Category].PrefabClasses;
	return nullptr;
}

int32 APossessionGameGameModeBase::GetRunFinishedTime() const { return FMath::FloorToInt(GetWorld()->GetTimeSeconds() - RunStartTime); }

void APossessionGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	bIsArenaMode = GetWorld()->GetMapName().Contains("Arena");
	GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestIsArenaMode.BindUObject(this, &APossessionGameGameModeBase::GetIsArenaMode);
	InitAfterPSOPrecache();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestCurrentTimer.BindUObject(this, &APossessionGameGameModeBase::GetRunFinishedTime);
}

void APossessionGameGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	if (const TObjectPtr<USaveSlot> SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot(); !SaveSlot)
	{
		GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadSaveSlot(FString("SaveSlot1"));
		GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadRunSaveSlot(FString("RunSaveSlot1"));
	}
	Cast<UPossessionGameInstance>(GetGameInstance())->InitGameInstanceManagers();
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

void APossessionGameGameModeBase::InitAfterPSOPrecache()
{
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	const TObjectPtr<USaveSlot> SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	(void)DelegateManager->RequestAddAbilitiesToUpgradePool.ExecuteIfBound(SaveSlot);
	GetWorld()->GetSubsystem<UMarkerManager>()->Init(MarkerClass, MarkerTypes);
	LoadSettingsData();
	GetWorld()->GetSubsystem<UVariationManager>()->Init(SaveSlot);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>();
	WidgetManager->Init(WidgetClasses);
	if (FShaderPipelineCache::NumPrecompilesRemaining() > 0) WidgetManager->GetWidget<ULoadingScreenUI>(EWidget::LoadingScreenUI)->Init(nullptr);
	GetWorld()->GetSubsystem<UUpgradeManager>()->Init(Upgrades);
	GetWorld()->GetSubsystem<UWaveManager>()->Init(DelegateManager, GameModesData);
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->Init(DelegateManager, TargetEQS);
	GetWorld()->GetSubsystem<UPoolingManager>()->Init(PoolObjects, ThrowablesData);
	GetWorld()->GetSubsystem<UPostProcessManager>()->Init(PostProcessMaterials);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
	SpawnArtifact();
	DelegateManager->OnGameModeInitialized.Broadcast();
	DelegateManager->OnFirstGameModeInitiated.BindUObject(this, &APossessionGameGameModeBase::SetRunStart);
	DelegateManager->OnRunFinished.BindUObject(this, &APossessionGameGameModeBase::GetRunFinishedTime);
	if (!bIsArenaMode && RunSaveSlot->GetCurrentChapter() > 1)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnEnemy(RunSaveSlot->GetCurrentCharacter(), EGameMode::None, nullptr, nullptr);
	}
	else GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnEnemy(CharacterToSpawn, EGameMode::None, nullptr, nullptr);
}

void APossessionGameGameModeBase::SetRunStart() { RunStartTime = GetWorld()->GetTimeSeconds(); }

/*void APossessionGameGameModeBase::PlayCutscene(const EGameMode InGameMode)
{
	if (InGameMode == EGameMode::BossMode)
	{
		const TObjectPtr<ALevelSequenceActor> SequenceActor = Cast<ALevelSequenceActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSequenceActor::StaticClass()));
		if (!SequenceActor) return;
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->SetVisibility(ESlateVisibility::Collapsed);
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD)->SetVisibility(ESlateVisibility::Collapsed);
		SequenceActor->GetSequencePlayer()->Play();
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnCutsceneStateChange.Broadcast(true);
		GetWorldTimerManager().SetTimer(FCutsceneTimerHandle, this, &APossessionGameGameModeBase::CutsceneEnded, SequenceActor->GetSequencePlayer()->GetEndTime().AsSeconds(), false);
	}
}

void APossessionGameGameModeBase::CutsceneEnded() const
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->SetVisibility(ESlateVisibility::HitTestInvisible);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD)->SetVisibility(ESlateVisibility::HitTestInvisible);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnCutsceneStateChange.Broadcast(false);
}*/
