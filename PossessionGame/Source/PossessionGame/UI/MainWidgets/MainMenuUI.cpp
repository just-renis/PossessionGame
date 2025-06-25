#include "MainMenuUI.h"
#include "SettingsUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/InvalidationBox.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UMainMenuUI::OpenUI()
{
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	if (SaveSlot->GetIsFirstLaunch())
	{
		SaveSlot->SetLaunchedOnce();
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

void UMainMenuUI::CloseUI() { Toggle(false); }

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtons();
	FirstLaunchAnimDelegate.BindUFunction(this, "TriggerMainMenuFadeInAnim");
}

void UMainMenuUI::Toggle(const bool bState)
{
	SetIsEnabled(bState);
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

void UMainMenuUI::TriggerMainMenuFadeInAnim()
{
	UnbindFromAnimationFinished(FirstLaunchAnim, FirstLaunchAnimDelegate);
	ToggleMenuButtons(true);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeIn);
	PlayAnimation(MainMenuFadeInAnim, GetAnimationCurrentTime(MainMenuFadeInAnim));
}

void UMainMenuUI::TriggerMainMenuFadeOutAnim()
{
	ToggleMenuButtons(false);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeOut);
	PlayAnimation(MainMenuFadeInAnim, GetAnimationCurrentTime(MainMenuFadeInAnim), 1, EUMGSequencePlayMode::Reverse);
}

void UMainMenuUI::ToggleMenuButtons(const bool bState) const
{
	if (PlayButton) PlayButton->SetIsEnabled(bState);
	if (SettingsButton) SettingsButton->SetIsEnabled(bState);
	if (QuitButton) QuitButton->SetIsEnabled(bState);
}

void UMainMenuUI::Play() { UGameplayStatics::OpenLevel(GetWorld(), "Level_1"); }

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
	TriggerMainMenuFadeOutAnim();
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->OpenUI();
}
