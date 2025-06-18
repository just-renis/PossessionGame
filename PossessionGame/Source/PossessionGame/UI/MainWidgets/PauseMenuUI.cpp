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

void UPauseMenuUI::ChangeUIToDeathMenu()
{
	if (ContinueButton)
	{
		ContinueButton->Get()->OnClicked.RemoveAll(this);
		ContinueButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::RestartLevel);
		ContinueButton->SetText(FText::FromString("Restart Level"));
	}
	if (SettingsButton)
	{
		SettingsButton->Get()->OnClicked.RemoveAll(this);
		SettingsButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::ReturnToMainMenu);
		SettingsButton->SetText(FText::FromString("Return To Menu"));
	}
	if (ReturnToMenuButton)
	{
		ReturnToMenuButton->Get()->OnClicked.RemoveAll(this);
		ReturnToMenuButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::QuitGame);
		ReturnToMenuButton->SetText(FText::FromString("Quit Game"));
	}
}

void UPauseMenuUI::CloseUI()
{
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
	SetIsEnabled(bState);
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>(); 
	if (WidgetManager->ContainsWidget(EWidget::PlayerHUD)) WidgetManager->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
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

void UPauseMenuUI::ReturnToMainMenu() { UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenuLevel")); }

void UPauseMenuUI::OpenSettings()
{
	SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->OpenUI();
}

void UPauseMenuUI::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, *UGameplayStatics::GetCurrentLevelName(GetWorld(), true));
}

void UPauseMenuUI::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
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
