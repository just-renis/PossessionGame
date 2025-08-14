#include "MainMenuUI.h"
#include "SaveSlotMenuUI.h"
#include "SettingsUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/InvalidationBox.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/MetaSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UMainMenuUI::OpenUI()
{
	const TObjectPtr<UMetaSaveSlot> MetaSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetMetaSaveSlot();
	if (MetaSaveSlot->GetIsFirstLaunch())
	{
		MetaSaveSlot->SetLaunchedOnce();
		BindToAnimationFinished(FirstLaunchAnim, FirstLaunchAnimDelegate);
		PlayAnimation(FirstLaunchAnim);
	}
	else
	{
		if (BlackScreenInv) BlackScreenInv->SetRenderOpacity(0.f);
		TriggerMainMenuFadeInAnim();
	}
	Toggle(true);
}

void UMainMenuUI::CloseUI() { if (bIsQuitPopupOpen) TriggerQuitPopupFadeOut(); }

void UMainMenuUI::TriggerMainMenuFadeInAnim()
{
	UnbindFromAnimationFinished(FirstLaunchAnim, FirstLaunchAnimDelegate);
	ToggleMenuButtons(true);
	PlayAnimation(MainMenuFadeInAnim, GetAnimationCurrentTime(MainMenuFadeInAnim));
}

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtons();
	FirstLaunchAnimDelegate.BindUFunction(this, "TriggerMainMenuFadeInAnim");
}

void UMainMenuUI::Toggle(const bool bState)
{
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
	if (QuitConfirmationPopup->GetAcceptButton()) QuitConfirmationPopup->GetAcceptButton()->Get()->OnClicked.AddDynamic(this, &UMainMenuUI::QuitGame);
	if (QuitConfirmationPopup->GetDeclineButton()) QuitConfirmationPopup->GetDeclineButton()->Get()->OnClicked.AddDynamic(this, &UMainMenuUI::TriggerQuitPopupFadeOut);
}

void UMainMenuUI::TriggerMainMenuFadeOutAnim()
{
	ToggleMenuButtons(false);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeOut);
	PlayAnimation(MainMenuFadeInAnim, GetAnimationCurrentTime(MainMenuFadeInAnim), 1, EUMGSequencePlayMode::Reverse);
}

void UMainMenuUI::ToggleMenuButtons(const bool bState) const
{
	if (PlayButton) PlayButton->SetButtonVisibility(bState);
	if (SettingsButton) SettingsButton->SetButtonVisibility(bState);
	if (QuitButton) QuitButton->SetButtonVisibility(bState);
}

void UMainMenuUI::Play()
{
	ToggleMenuButtons(false);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USaveSlotMenuUI>(EWidget::SaveSlotMenuUI)->OpenUI();
}

void UMainMenuUI::OpenSettings()
{
	TriggerMainMenuFadeOutAnim();
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->OpenUI();
}

void UMainMenuUI::QuitGame() { UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false); }

void UMainMenuUI::TriggerQuitPopupFadeIn()
{
	if (!QuitConfirmationPopup) return;
	bIsQuitPopupOpen = true;
	TriggerMainMenuFadeOutAnim();
	QuitConfirmationPopup->PlayPopupFadeInAnim(true);
}

void UMainMenuUI::TriggerQuitPopupFadeOut()
{
	if (!QuitConfirmationPopup) return;
	bIsQuitPopupOpen = false;
	TriggerMainMenuFadeInAnim();
	QuitConfirmationPopup->PlayPopupFadeInAnim(false);
}
