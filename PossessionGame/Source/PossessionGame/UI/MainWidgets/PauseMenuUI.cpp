#include "PauseMenuUI.h"
#include "SettingsUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UPauseMenuUI::OpenUI() { Toggle(true); }

void UPauseMenuUI::ChangeUIToDeathMenu()
{
	if (ContinueButton)
	{
		ContinueButton->OnClicked.RemoveAll(this);
		ContinueButton->OnClicked.AddDynamic(this, &UPauseMenuUI::RestartLevel);
	}
	if (ContinueText) ContinueText->SetText(FText::FromString("Restart Level"));
	if (SettingsButton)
	{
		SettingsButton->OnClicked.RemoveAll(this);
		SettingsButton->OnClicked.AddDynamic(this, &UPauseMenuUI::ReturnToMainMenu);
	}
	if (SettingsText) SettingsText->SetText(FText::FromString("Return To Menu"));
	if (ReturnToMainMenuButton)
	{
		ReturnToMainMenuButton->OnClicked.RemoveAll(this);
		ReturnToMainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuUI::QuitGame);
	}
	if (ReturnToMainMenuText) ReturnToMainMenuText->SetText(FText::FromString("Quit Game"));
}

void UPauseMenuUI::CloseUI() { Toggle(false); }

void UPauseMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtons();
}

void UPauseMenuUI::Toggle(bool bState)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeUIOnly());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::PauseMenuUI : EWidget::None);
}

void UPauseMenuUI::SetupButtons()
{
	if (ContinueButton) ContinueButton->OnClicked.AddDynamic(this, &UPauseMenuUI::CloseUI);
	if (ReturnToMainMenuButton) ReturnToMainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuUI::ReturnToMainMenu);
	if (SettingsButton) SettingsButton->OnClicked.AddDynamic(this, &UPauseMenuUI::OpenSettings);
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
