#include "PauseMenuUI.h"
#include "PlayerHUD.h"
#include "SettingsUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UPauseMenuUI::OpenUI() { Toggle(true); }

void UPauseMenuUI::CloseUI()
{
	if (ReturnToMainMenuPopup->GetVisibility() == ESlateVisibility::SelfHitTestInvisible && !ReturnToMainMenuPopup->GetIsAnimationPlaying())
	{
		TriggerReturnToMainMenuPopupFadeOut();
		return;
	}
	if (IsAnimationPlaying(FadeOutAnim)) return;
	BindToAnimationFinished(FadeOutAnim, FadeOutAnimDelegate);
	PlayAnimation(FadeOutAnim);
}

void UPauseMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeOutAnimDelegate.BindDynamic(this, &UPauseMenuUI::FadeOutFinished);
	SetupButtons();
}

void UPauseMenuUI::Toggle(const bool bState)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (ContinueButton) ContinueButton->SetButtonVisibility(bState);
	if (SettingsButton) SettingsButton->SetButtonVisibility(bState);
	if (ReturnToMenuButton) ReturnToMenuButton->SetButtonVisibility(bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>(); 
	if (WidgetManager->ContainsWidget(EWidget::PlayerHUD)) WidgetManager->GetWidget<UPlayerHUD>(EWidget::PlayerHUD, 0)->PlayFadeOutAnim(bState);
	if (bState) PlayAnimation(FadeInAnim);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	WidgetManager->SetCurrentStateOfUI(bState ? EWidget::PauseMenuUI : EWidget::None);
}

void UPauseMenuUI::SetupButtons()
{
	if (ContinueButton) ContinueButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::CloseUI);
	if (ReturnToMenuButton) ReturnToMenuButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::TriggerReturnToMainMenuPopupFadeIn);
	if (SettingsButton) SettingsButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::OpenSettings);
	if (ReturnToMainMenuPopup->GetAcceptButton()) ReturnToMainMenuPopup->GetAcceptButton()->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::ReturnToMainMenu);
	if (ReturnToMainMenuPopup->GetDeclineButton()) ReturnToMainMenuPopup->GetDeclineButton()->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::TriggerReturnToMainMenuPopupFadeOut);

}

void UPauseMenuUI::SetButtonsState(const bool bState) const
{
	if (ContinueButton) ContinueButton->Get()->SetIsEnabled(bState);
	if (ReturnToMenuButton) ReturnToMenuButton->Get()->SetIsEnabled(bState);
	if (SettingsButton) SettingsButton->Get()->SetIsEnabled(bState);
}

void UPauseMenuUI::ReturnToMainMenu() { UGameplayStatics::OpenLevel(GetWorld(), "MainMenu"); }

void UPauseMenuUI::OpenSettings()
{
	SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI, 2)->OpenUI();
}

void UPauseMenuUI::TriggerReturnToMainMenuPopupFadeIn()
{
	if (!ReturnToMainMenuPopup) return;
	SetButtonsState(false);
	ReturnToMainMenuPopup->PlayPopupFadeInAnim(true);
}

void UPauseMenuUI::TriggerReturnToMainMenuPopupFadeOut()
{
	if (!ReturnToMainMenuPopup) return;
	SetButtonsState(true);
	ReturnToMainMenuPopup->PlayPopupFadeInAnim(false);
}

void UPauseMenuUI::FadeOutFinished()
{
	UnbindAllFromAnimationFinished(FadeOutAnim);
	Toggle(false);
}
