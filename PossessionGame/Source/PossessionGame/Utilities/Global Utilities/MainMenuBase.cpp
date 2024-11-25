#include "MainMenuBase.h"
#include "SettingsSave.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/MainMenuUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void AMainMenuBase::BeginPlay()
{
	Super::BeginPlay();
	LoadSettingsData();
	TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>();
	WidgetManager->Init(WidgetClasses);
	WidgetManager->GetWidget<UMainMenuUI>(EWidget::MainMenuUI)->OpenUI();
}

void AMainMenuBase::LoadSettingsData()
{
	FString SettingsSlot = "SettingsSlot";
	if (UGameplayStatics::DoesSaveGameExist(SettingsSlot, 0))
	{
		TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot(SettingsSlot, 0));
		SavedSettings->LoadSettings(GetWorld());
	}
	else
	{
		TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::CreateSaveGameObject(SettingsSaveClass));
		SavedSettings->LoadSettingsOnFirstLaunch(GetWorld());
		UGameplayStatics::SaveGameToSlot(SavedSettings, SettingsSlot, 0);
	}
}
