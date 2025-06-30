#include "MainMenuBase.h"
#include "SettingsSave.h"
#include "ShaderPipelineCache.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/MainWidgets/LoadingScreenUI.h"
#include "PossessionGame/UI/MainWidgets/MainMenuUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void AMainMenuBase::BeginPlay()
{
	Super::BeginPlay();
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>();
	WidgetManager->Init(WidgetClasses);
	if (FShaderPipelineCache::NumPrecompilesRemaining() > 0)
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ChangeMusicPlayerState.ExecuteIfBound(false);
		WidgetManager->GetWidget<ULoadingScreenUI>(EWidget::LoadingScreenUI)->Init([this]{ InitAfterPSOPrecache(); });
	}
	else InitAfterPSOPrecache();
}

void AMainMenuBase::LoadSettingsData() const
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

void AMainMenuBase::InitAfterPSOPrecache() const
{
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ChangeMusicPlayerState.ExecuteIfBound(true);
	LoadSettingsData();
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UMainMenuUI>(EWidget::MainMenuUI)->OpenUI();
}
