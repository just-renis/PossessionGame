#include "TutorialModeBase.h"
#include "SettingsSave.h"
#include "ShaderPipelineCache.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/MainWidgets/LoadingScreenUI.h"
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

void ATutorialModeBase::InitAfterPSOPrecache() const
{
	if (FShaderPipelineCache::NumPrecompilesRemaining() > 0) GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<ULoadingScreenUI>(EWidget::LoadingScreenUI)->Init(nullptr);
}
