#include "SettingsUI.h"
#include "AudioSettingsUI.h"
#include "ControlsSettingsUI.h"
#include "GraphicsSettingsUI.h"
#include "MainMenuUI.h"
#include "PauseMenuUI.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/AlertPopupUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void USettingsUI::OpenUI()
{
	SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(EWidget::SettingsUI);
}

void USettingsUI::CloseUI()
{
	SetSavedSettings();
	SetVisibility(ESlateVisibility::Collapsed);
	if (UGameplayStatics::GetCurrentLevelName(this, true).Equals("MainMenuLevel")) GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UMainMenuUI>(EWidget::MainMenuUI)->OpenUI();
	else GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->OpenUI();
}

void USettingsUI::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateDefaultSettings();
	SetupUIEvents();
}

void USettingsUI::SetupUIEvents()
{
	if (GraphicsSettings) GraphicsSettings->SetupUIEvents();
	if (ResetKeyBindsButton) ResetKeyBindsButton->OnClicked.AddDynamic(this, &USettingsUI::ResetKeyBindsToDefault);
	if (ApplySettingsButton) ApplySettingsButton->OnClicked.AddDynamic(this, &USettingsUI::ApplySettings);
	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &USettingsUI::CloseUI);
	if (MainTabButton) MainTabButton->OnClicked.AddDynamic(this, &USettingsUI::OpenMainTab);
	if (GraphicsTabButton) GraphicsTabButton->OnClicked.AddDynamic(this, &USettingsUI::OpenGraphicsTab);
	if (ControlsTabButton) ControlsTabButton->OnClicked.AddDynamic(this, &USettingsUI::OpenControlsTab);
	if (DuplicateKeyBindsAlert->GetAlertButton()) DuplicateKeyBindsAlert->GetAlertButton()->OnClicked.AddDynamic(this, &USettingsUI::CloseDuplicateKeyBindsPopup);
}

void USettingsUI::UpdateDefaultSettings()
{
	if (GraphicsSettings) GraphicsSettings->UpdateDefaultGraphicsSettings();
	if (MainSettings) MainSettings->UpdateDefaultAudioSettings();
}

void USettingsUI::SetSavedSettings()
{
	if (GraphicsSettings) GraphicsSettings->SetSavedSettings();
	if (MainSettings) MainSettings->SetSavedSettings();
	if (ControlsSettings) ControlsSettings->SetSavedSettings();
}

void USettingsUI::OpenDuplicateKeyBindsPopup() const
{
	UE_LOG(LogTemp, Warning, TEXT("OpenDuplicateKeyBindsPopup"));
	if (DuplicateKeyBindsAlert) DuplicateKeyBindsAlert->PlayPopupFadeInAnim(true);
}

void USettingsUI::ApplySettings()
{
	if (ControlsSettings && !ControlsSettings->ApplySettings()) OpenDuplicateKeyBindsPopup();
	else
	{
		if (GraphicsSettings) GraphicsSettings->ApplySettings();
		if (MainSettings) MainSettings->ApplySettings();
	}
}

void USettingsUI::OpenMainTab() { if (TabsSwitcher) TabsSwitcher->SetActiveWidgetIndex(0); }

void USettingsUI::OpenGraphicsTab() { if (TabsSwitcher) TabsSwitcher->SetActiveWidgetIndex(1); }

void USettingsUI::OpenControlsTab() { if (TabsSwitcher) TabsSwitcher->SetActiveWidgetIndex(2); }

void USettingsUI::ResetKeyBindsToDefault() { if (ControlsSettings) ControlsSettings->ResetKeyBindsToDefault(); }

void USettingsUI::CloseDuplicateKeyBindsPopup()
{
	UE_LOG(LogTemp, Warning, TEXT("CloseDuplicateKeyBindsPopup"));
	if (DuplicateKeyBindsAlert) DuplicateKeyBindsAlert->PlayPopupFadeInAnim(false);
}
