#include "MainMenuUI.h"
#include "ConfirmationPopupUI.h"
#include "MainMenuButtonUI.h"
#include "SettingsUI.h"
#include "Components/Button.h"
#include "Components/InvalidationBox.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UMainMenuUI::OpenUI() { Toggle(true); }

void UMainMenuUI::CloseUI() { Toggle(false); }

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtons();
}

void UMainMenuUI::Toggle(bool bState)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::MainMenuUI : EWidget::None);
}

void UMainMenuUI::SetupButtons()
{
	if (PlayButton)
	{
		PlayButton->Get()->OnClicked.AddDynamic(this, &UMainMenuUI::Play);
		PlayButton->SetupEvents();
	}
	if (SettingsButton)
	{
		SettingsButton->Get()->OnClicked.AddDynamic(this, &UMainMenuUI::OpenSettings);
		SettingsButton->SetupEvents();
	}
	if (QuitButton)
	{
		QuitButton->Get()->OnClicked.AddDynamic(this, &UMainMenuUI::TriggerQuitPopup);
		QuitButton->SetupEvents();
	}
	if (QuitConfirmationPopup->GetAcceptButton()) QuitConfirmationPopup->GetAcceptButton()->OnClicked.AddDynamic(this, &UMainMenuUI::QuitGame);
	if (QuitConfirmationPopup->GetDeclineButton()) QuitConfirmationPopup->GetDeclineButton()->OnClicked.AddDynamic(this, &UMainMenuUI::TriggerQuitPopup);
}

void UMainMenuUI::TriggerMainMenuFadeOutAnim()
{
	MainMenuFadeOutState ? PlayAnimation(MainMenuFadeOut, GetAnimationCurrentTime(MainMenuFadeOut), 1, EUMGSequencePlayMode::Reverse) : PlayAnimation(MainMenuFadeOut, GetAnimationCurrentTime(MainMenuFadeOut));
	MainMenuFadeOutState = !MainMenuFadeOutState;
}

void UMainMenuUI::TriggerQuitFadeInAnim()
{
	QuitConfirmationPopup->PlayPopupFadeInAnim(QuitFadeInState);
	QuitFadeInState = !QuitFadeInState;
}

void UMainMenuUI::Play() { UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Levels/Chapter1/Levels/ZoneA/Level_1")); }

void UMainMenuUI::QuitGame() { UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false); }

void UMainMenuUI::TriggerQuitPopup()
{
	MainMenuInv->SetVisibility(MainMenuInv->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::SelfHitTestInvisible);
	TriggerMainMenuFadeOutAnim();
	TriggerQuitFadeInAnim();
}

void UMainMenuUI::OpenSettings()
{
	SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->OpenUI();
}
