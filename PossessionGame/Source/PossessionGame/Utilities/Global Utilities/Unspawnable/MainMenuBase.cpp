#include "MainMenuBase.h"

#include "PossessionGameInstance.h"
#include "SettingsSave.h"
#include "ShaderPipelineCache.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/MainWidgets/GameLaunchUI.h"
#include "PossessionGame/UI/MainWidgets/LoadingScreenUI.h"
#include "PossessionGame/UI/MainWidgets/MainMenuUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void AMainMenuBase::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<UWidgetManager>()->Init(WidgetClasses);
	ShowLoadingScreen();
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
		UGameplayStatics::SaveGameToSlot(SavedSettings, SettingsSlot, 0);
	}
}

void AMainMenuBase::InitAfterPSOPrecache() const
{
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ChangeMusicPlayerState.ExecuteIfBound(true);
	LoadSettingsData();
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UMainMenuUI>(EWidget::MainMenuUI, 0)->OpenUI();
}

void AMainMenuBase::ShowLoadingScreen()
{
	if (FShaderPipelineCache::NumPrecompilesRemaining() > 0)
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ChangeMusicPlayerState.ExecuteIfBound(false);
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<ULoadingScreenUI>(EWidget::LoadingScreenUI, 3)->Init([this]{ ShowGameLaunchUI(); });
	}
	else ShowGameLaunchUI();
}

void AMainMenuBase::ShowGameLaunchUI()
{
	const TObjectPtr<UPossessionGameInstance> GameInstance = Cast<UPossessionGameInstance>(GetGameInstance());
	if (!GameInstance->GetIsMainMenuLaunchedOnce())
	{
		GameInstance->SetMainMenuLaunchedOnce();
		GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UGameLaunchUI>(EWidget::GameLaunchUI, 3);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameLaunchFinished.BindUObject(this, &AMainMenuBase::InitAfterPSOPrecache);
	}
	else InitAfterPSOPrecache();
}
