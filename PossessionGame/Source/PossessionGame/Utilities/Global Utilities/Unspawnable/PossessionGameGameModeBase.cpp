#include "PossessionGameGameModeBase.h"
#include "PossessionGameInstance.h"
#include "SettingsSave.h"
#include "ShaderPipelineCache.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/LoadingScreenUI.h"
#include "PossessionGame/UI/MainWidgets/UpgradeSelectionUI.h"
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

void APossessionGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	InitAfterPSOPrecache();
	//GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->OpenUI();
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
	const TObjectPtr<UPossessionGameInstance> GameInstance = Cast<UPossessionGameInstance>(GetGameInstance());
	const TObjectPtr<USaveSlot> SaveSlot = GameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	GetWorld()->GetSubsystem<UMarkerManager>()->Init(MarkerClass, MarkerTypes);
	LoadSettingsData();
	GetWorld()->GetSubsystem<UVariationManager>()->Init(SaveSlot);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>();
	WidgetManager->Init(WidgetClasses);
	if (FShaderPipelineCache::NumPrecompilesRemaining() > 0) WidgetManager->GetWidget<ULoadingScreenUI>(EWidget::LoadingScreenUI)->Init(nullptr);
	GetWorld()->GetSubsystem<UUpgradeManager>()->Init(Upgrades);
	GameInstance->InitGameInstanceManagers();
	GetWorld()->GetSubsystem<UWaveManager>()->Init(DelegateManager, GameModesData);
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->Init(DelegateManager, TargetEQS);
	GetWorld()->GetSubsystem<UPoolingManager>()->Init(PoolObjects);
	GetWorld()->GetSubsystem<UPostProcessManager>()->Init(PostProcessMaterials);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
	SpawnArtifact();
	DelegateManager->OnGameModeInitialized.Broadcast();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnEnemy(CharacterToSpawn, EGameMode::None, false, nullptr);
}

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
