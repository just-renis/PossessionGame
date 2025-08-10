#include "TutorialModeBase.h"

#include "PossessionGameInstance.h"
#include "SettingsSave.h"
#include "ShaderPipelineCache.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/MainWidgets/LoadingScreenUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Utilities/Subsystems/WaveManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void ATutorialModeBase::BeginPlay()
{
	Super::BeginPlay();
	InitAfterPSOPrecache();
}

void ATutorialModeBase::LoadSettingsData() const
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

void ATutorialModeBase::InitAfterPSOPrecache()
{
	const TObjectPtr<UPossessionGameInstance> GameInstance = Cast<UPossessionGameInstance>(GetGameInstance());
	const TObjectPtr<USaveSlot> SaveSlot = GameInstance->GetSaveSlot();
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	LoadSettingsData();
	GetWorld()->GetSubsystem<UVariationManager>()->Init(SaveSlot);
	GetWorld()->GetSubsystem<UMarkerManager>()->Init(MarkerClass);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>();
	WidgetManager->Init(WidgetClasses);
	if (FShaderPipelineCache::NumPrecompilesRemaining() > 0) WidgetManager->GetWidget<ULoadingScreenUI>(EWidget::LoadingScreenUI)->Init(nullptr);
	GameInstance->InitGameInstanceManagers();
	GetWorld()->GetSubsystem<UUpgradeManager>()->Init(Upgrades);
	GetWorld()->GetSubsystem<UWaveManager>()->Init(DelegateManager, GameModesData);
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->Init(DelegateManager, TargetEQS);
	GetWorld()->GetSubsystem<UPoolingManager>()->Init(PoolObjects);
	GetWorld()->GetSubsystem<UPostProcessManager>()->Init(PostProcessMaterials);
}
