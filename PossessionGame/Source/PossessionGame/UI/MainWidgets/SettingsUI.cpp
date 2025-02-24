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
	GraphicsSettings->SetupUIEvents();
	if (ResetKeyBindsButton) ResetKeyBindsButton->OnClicked.AddDynamic(this, &USettingsUI::ResetKeyBindsToDefault);
	if (ApplySettingsButton) ApplySettingsButton->OnClicked.AddDynamic(this, &USettingsUI::ApplySettings);
	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &USettingsUI::CloseUI);
	if (GraphicsTabButton) GraphicsTabButton->OnClicked.AddDynamic(this, &USettingsUI::OpenGraphicsTab);
	if (AudioTabButton) AudioTabButton->OnClicked.AddDynamic(this, &USettingsUI::OpenAudioTab);
	if (ControlsTabButton) ControlsTabButton->OnClicked.AddDynamic(this, &USettingsUI::OpenControlsTab);
	if (LanguageTabButton) LanguageTabButton->OnClicked.AddDynamic(this, &USettingsUI::OpenLanguageTab);
	if (DuplicateKeyBindsAlert->GetAlertButton()) DuplicateKeyBindsAlert->GetAlertButton()->OnClicked.AddDynamic(this, &USettingsUI::CloseDuplicateKeyBindsPopup);
}

void USettingsUI::UpdateDefaultSettings()
{
	GraphicsSettings->UpdateDefaultGraphicsSettings();
	AudioSettings->UpdateDefaultAudioSettings();
}

void USettingsUI::SetSavedSettings()
{
	GraphicsSettings->SetSavedSettings();
	AudioSettings->SetSavedSettings();
	ControlsSettings->SetSavedSettings();
}

void USettingsUI::ApplySettings()
{
	if (!ControlsSettings->ApplySettings())
	{
		DuplicateKeyBindsAlert->PlayPopupFadeInAnim(true);
		PlayAnimation(SettingsFadeIn, 0, 1, EUMGSequencePlayMode::Reverse);
	}
	else
	{
		GraphicsSettings->ApplySettings();
		AudioSettings->ApplySettings();
	}
}

void USettingsUI::OpenGraphicsTab() { TabsSwitcher->SetActiveWidgetIndex(0); }

void USettingsUI::OpenAudioTab() { TabsSwitcher->SetActiveWidgetIndex(1); }

void USettingsUI::OpenControlsTab() { TabsSwitcher->SetActiveWidgetIndex(2); }

void USettingsUI::OpenLanguageTab() { TabsSwitcher->SetActiveWidgetIndex(3); }

void USettingsUI::ResetKeyBindsToDefault() { ControlsSettings->ResetKeyBindsToDefault(); }

void USettingsUI::CloseDuplicateKeyBindsPopup()
{
	DuplicateKeyBindsAlert->PlayPopupFadeInAnim(false);
	PlayAnimation(SettingsFadeIn);
}
