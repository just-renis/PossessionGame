#include "SettingsUI.h"
#include "AudioSettingsUI.h"
#include "ControlsSettingsUI.h"
#include "GraphicsSettingsUI.h"
#include "MainMenuUI.h"
#include "PauseMenuUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/InvalidationBox.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/AlertPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/KeyBindingUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void USettingsUI::BeginRebind(const TObjectPtr<UKeyBindingUI>& InTargetKeyWidget)
{
	ToggleRebindPanel(true);
	if (CurrentKeyWidget) CurrentKeyWidget->CancelRebind();
	CurrentKeyWidget = InTargetKeyWidget;
	SetVisibility(ESlateVisibility::Visible);
	SetKeyboardFocus();
}

void USettingsUI::OpenUI()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeIn);
	SetButtonsVisibilityState(true);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(SettingsFadeInAnim);
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(EWidget::SettingsUI);
}

void USettingsUI::CloseUI()
{
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(EWidget::None);
	SetButtonsVisibilityState(false);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeOut);
	PlayAnimation(SettingsFadeInAnim, 0, 1, EUMGSequencePlayMode::Reverse);
	BindToAnimationFinished(SettingsFadeInAnim, SettingsPanelFadeOutDelegate);
	SetSavedSettings();
}

USettingsUI::USettingsUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) { SetIsFocusable(true); }

FReply USettingsUI::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!CurrentKeyWidget) return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	const FInputChord Chord(InKeyEvent.GetKey());
	CurrentKeyWidget->OnKeySelected(Chord);
	CurrentKeyWidget = nullptr;
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ToggleRebindPanel(false);
	return FReply::Handled();
}

FReply USettingsUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!CurrentKeyWidget) return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	const FInputChord Chord(InMouseEvent.GetEffectingButton());
	CurrentKeyWidget->OnKeySelected(Chord);
	CurrentKeyWidget = nullptr;
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ToggleRebindPanel(false);
	return FReply::Handled();
}

void USettingsUI::NativeConstruct()
{
	Super::NativeConstruct();
	RebindPanelFadeOutDelegate.BindDynamic(this, &USettingsUI::RebindPanelFadeOutFinished);
	SettingsPanelFadeOutDelegate.BindDynamic(this, &USettingsUI::SettingsFadeOutFinished);
	if (ControlsSettings) ControlsSettings->Setup(this);
	UpdateDefaultSettings();
	SetupUIEvents();
}

void USettingsUI::SetupUIEvents()
{
	if (GraphicsSettings) GraphicsSettings->SetupUIEvents();
	if (ResetKeyBindsButton) ResetKeyBindsButton->Get()->OnClicked.AddDynamic(this, &USettingsUI::ResetKeyBindsToDefault);
	if (ApplySettingsButton) ApplySettingsButton->Get()->OnClicked.AddDynamic(this, &USettingsUI::ApplySettings);
	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &USettingsUI::CloseWithButton);
	if (MainTabButton)
	{
		MainTabButton->Get()->SetIsEnabled(false);
		MainTabButton->Get()->OnClicked.AddDynamic(this, &USettingsUI::OpenMainTab);
	}
	if (GraphicsTabButton) GraphicsTabButton->Get()->OnClicked.AddDynamic(this, &USettingsUI::OpenGraphicsTab);
	if (ControlsTabButton) ControlsTabButton->Get()->OnClicked.AddDynamic(this, &USettingsUI::OpenControlsTab);
	if (DuplicateKeyBindsAlert->GetAlertButton()) DuplicateKeyBindsAlert->GetAlertButton()->Get()->OnClicked.AddDynamic(this, &USettingsUI::CloseDuplicateKeyBindsPopup);
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

void USettingsUI::OpenDuplicateKeyBindsPopup() const { if (DuplicateKeyBindsAlert) DuplicateKeyBindsAlert->PlayPopupFadeInAnim(true); }

void USettingsUI::ToggleRebindPanel(const bool bState)
{
	if (!RebindPanelInv) return;
	if (bState)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeIn);
		RebindPanelInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeOut);
		BindToAnimationFinished(RebindPanelFadeInAnim, RebindPanelFadeOutDelegate);
	}
	PlayAnimation(RebindPanelFadeInAnim, 0, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void USettingsUI::SetButtonsVisibilityState(const bool bState) const
{
	if (ApplySettingsButton) ApplySettingsButton->SetButtonVisibility(bState);
	if (GraphicsTabButton) GraphicsTabButton->SetButtonVisibility(bState);
	if (MainTabButton) MainTabButton->SetButtonVisibility(bState);
	if (ControlsTabButton) ControlsTabButton->SetButtonVisibility(bState);
	if (ResetKeyBindsButton) ResetKeyBindsButton->SetButtonVisibility(bState);
}

void USettingsUI::RebindPanelFadeOutFinished()
{
	UnbindAllFromAnimationFinished(RebindPanelFadeInAnim);
	if (RebindPanelInv) RebindPanelInv->SetVisibility(ESlateVisibility::Collapsed);
}

void USettingsUI::SettingsFadeOutFinished()
{
	UnbindAllFromAnimationFinished(SettingsFadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
	if (UGameplayStatics::GetCurrentLevelName(this, true).Equals("MainMenu")) GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UMainMenuUI>(EWidget::MainMenuUI, 0)->OpenUI();
	else GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI, 2)->OpenUI();
}

void USettingsUI::CloseWithButton()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
	CloseUI();
}

void USettingsUI::ApplySettings()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonSelect);
	if (ControlsSettings && !ControlsSettings->ApplySettings()) OpenDuplicateKeyBindsPopup();
	else
	{
		if (GraphicsSettings) GraphicsSettings->ApplySettings();
		if (MainSettings) MainSettings->ApplySettings();
	}
}

void USettingsUI::OpenMainTab()
{
	if (!TabsSwitcher) return;
	TabsSwitcher->SetActiveWidgetIndex(0);
	MainTabButton->Get()->SetIsEnabled(false);
	GraphicsTabButton->Get()->SetIsEnabled(true);
	ControlsTabButton->Get()->SetIsEnabled(true);
}

void USettingsUI::OpenGraphicsTab()
{
	if (!TabsSwitcher) return;
	TabsSwitcher->SetActiveWidgetIndex(1);
	MainTabButton->Get()->SetIsEnabled(true);
	GraphicsTabButton->Get()->SetIsEnabled(false);
	ControlsTabButton->Get()->SetIsEnabled(true);
}

void USettingsUI::OpenControlsTab()
{
	if (!TabsSwitcher) return;
	TabsSwitcher->SetActiveWidgetIndex(2);
	MainTabButton->Get()->SetIsEnabled(true);
	GraphicsTabButton->Get()->SetIsEnabled(true);
	ControlsTabButton->Get()->SetIsEnabled(false);
}

void USettingsUI::ResetKeyBindsToDefault()
{
	if (!ControlsSettings) return;
	ControlsSettings->ResetKeyBindsToDefault();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonSelect);
}

void USettingsUI::CloseDuplicateKeyBindsPopup()
{
	if (DuplicateKeyBindsAlert) DuplicateKeyBindsAlert->PlayPopupFadeInAnim(false);
}
