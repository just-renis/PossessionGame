#include "PauseMenuUI.h"
#include "SettingsUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UPauseMenuUI::OpenUI() { Toggle(true); }

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
	TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
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