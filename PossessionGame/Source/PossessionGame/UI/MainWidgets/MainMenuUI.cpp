#include "MainMenuUI.h"
#include "SettingsUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UMainMenuUI::OpenUI() { Toggle(true); }

void UMainMenuUI::CloseUI() { Toggle(false); }

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtons();
}

void UMainMenuUI::Toggle(const bool bState)
{
	SetIsEnabled(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::MainMenuUI : EWidget::None);
}

void UMainMenuUI::SetupButtons()
{
	if (PlayButton) PlayButton->Get()->OnClicked.AddDynamic(this, &UMainMenuUI::Play);
	if (SettingsButton) SettingsButton->Get()->OnClicked.AddDynamic(this, &UMainMenuUI::OpenSettings);
	if (QuitButton) QuitButton->Get()->OnClicked.AddDynamic(this, &UMainMenuUI::TriggerQuitPopupFadeIn);
	if (QuitConfirmationPopup->GetAcceptButton()) QuitConfirmationPopup->GetAcceptButton()->OnClicked.AddDynamic(this, &UMainMenuUI::QuitGame);
	if (QuitConfirmationPopup->GetDeclineButton()) QuitConfirmationPopup->GetDeclineButton()->OnClicked.AddDynamic(this, &UMainMenuUI::TriggerQuitPopupFadeOut);
}

void UMainMenuUI::TriggerMainMenuFadeInAnim()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeIn);
	PlayAnimation(MainMenuFadeInAnim, GetAnimationCurrentTime(MainMenuFadeInAnim));
}

void UMainMenuUI::TriggerMainMenuFadeOutAnim()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeOut);
	PlayAnimation(MainMenuFadeInAnim, GetAnimationCurrentTime(MainMenuFadeInAnim), 1, EUMGSequencePlayMode::Reverse);
}

void UMainMenuUI::Play() { UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Levels/Chapter1/Levels/Level_1")); }

void UMainMenuUI::QuitGame() { UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false); }

void UMainMenuUI::TriggerQuitPopupFadeIn()
{
	if (!QuitConfirmationPopup) return;
	TriggerMainMenuFadeOutAnim();
	QuitConfirmationPopup->PlayPopupFadeInAnim(true);
}

void UMainMenuUI::TriggerQuitPopupFadeOut()
{
	if (!QuitConfirmationPopup) return;
	TriggerMainMenuFadeInAnim();
	QuitConfirmationPopup->PlayPopupFadeInAnim(false);
}

void UMainMenuUI::OpenSettings()
{
	SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->OpenUI();
}
