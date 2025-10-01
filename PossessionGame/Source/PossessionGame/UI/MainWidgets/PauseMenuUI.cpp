#include "PauseMenuUI.h"
#include "PlayerHUD.h"
#include "SettingsUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UPauseMenuUI::OpenUI()
{
	bIsPauseMenuClosed = false;
	Toggle(true);
}

void UPauseMenuUI::CloseUI()
{
	if (bIsReturnToMainMenuPopupOpened)
	{
		TriggerReturnToMainMenuPopupFadeOut();
		return;
	}
	if (IsAnimationPlaying(FadeOutAnim)) return;
	bIsPauseMenuClosed = true;
	BindToAnimationFinished(FadeOutAnim, FadeOutAnimDelegate);
	PlayAnimation(FadeOutAnim);
}

void UPauseMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeOutAnimDelegate.BindDynamic(this, &UPauseMenuUI::FadeOutFinished);
	SetupButtons();
}

void UPauseMenuUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bReturnToMainMenuInProcess) return;
	TimerOfFadeOut += InDeltaTime;
	if (TimerOfFadeOut >= 1.5f)
	{
		TimerOfFadeOut = 0.f;
		GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(FLoadLevelData(false, false, false, FName("MainMenu")));
	}
}

void UPauseMenuUI::Toggle(const bool bState)
{
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>(); 
	if (!WidgetManager->GetIsExplanationOpened())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), bState);
		const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
		PlayerController->bShowMouseCursor = bState;
		if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
		else PlayerController->SetInputMode(FInputModeGameOnly());
	}
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	ContinueButton->SetButtonVisibility(bState);
	SettingsButton->SetButtonVisibility(bState);
	ReturnToMenuButton->SetButtonVisibility(bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	if (WidgetManager->ContainsWidget(EWidget::PlayerHUD)) WidgetManager->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	if (bState) PlayAnimation(FadeInAnim);
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

void UPauseMenuUI::ReturnToMainMenu()
{
	if (const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		if (const TObjectPtr<APawn> Pawn = PlayerController->GetPawn()) Pawn->DisableInput(PlayerController);
		PlayerController->bShowMouseCursor = false;
	}
	PlayAnimation(BlackScreenFadeOutAnim);
	bReturnToMainMenuInProcess = true;
}

void UPauseMenuUI::OpenSettings()
{
	SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->OpenUI();
}

void UPauseMenuUI::TriggerReturnToMainMenuPopupFadeIn()
{
	if (!ReturnToMainMenuPopup || bIsPauseMenuClosed) return;
	bIsReturnToMainMenuPopupOpened = true;
	SetButtonsState(false);
	ReturnToMainMenuPopup->PlayPopupFadeInAnim(true);
}

void UPauseMenuUI::TriggerReturnToMainMenuPopupFadeOut()
{
	if (!ReturnToMainMenuPopup) return;
	bIsReturnToMainMenuPopupOpened = false;
	SetButtonsState(true);
	ReturnToMainMenuPopup->PlayPopupFadeInAnim(false);
}

void UPauseMenuUI::FadeOutFinished()
{
	UnbindAllFromAnimationFinished(FadeOutAnim);
	Toggle(false);
}
