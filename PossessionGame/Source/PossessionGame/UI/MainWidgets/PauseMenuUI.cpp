#include "PauseMenuUI.h"

#include "PlayerHUD.h"
#include "SettingsUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
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

void UPauseMenuUI::CloseUI() { Toggle(false); }

void UPauseMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtons();
}

void UPauseMenuUI::Toggle(const bool bState)
{
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>(); 
	if (WidgetManager->ContainsWidget(EWidget::PlayerHUD)) WidgetManager->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	SetIsEnabled(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	WidgetManager->SetCurrentStateOfUI(bState ? EWidget::PauseMenuUI : EWidget::None);
}

void UPauseMenuUI::SetupButtons()
{
	if (ContinueButton) ContinueButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::CloseUI);
	if (ReturnToMenuButton) ReturnToMenuButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::ReturnToMainMenu);
	if (SettingsButton) SettingsButton->Get()->OnClicked.AddDynamic(this, &UPauseMenuUI::OpenSettings);
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
