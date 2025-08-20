#include "ConfirmationPopupUI.h"
#include "MainMenuButtonUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UConfirmationPopupUI::PlayPopupFadeInAnim(const bool bState)
{
	PlayAnimation(PopupFadeIn, GetAnimationCurrentTime(PopupFadeIn), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	if (bState) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else BindToAnimationFinished(PopupFadeIn, FadeOutDelegate);
	if (AcceptButton) AcceptButton->SetButtonVisibility(bState);
	if (DeclineButton) DeclineButton->SetButtonVisibility(bState);
}

void UConfirmationPopupUI::SetTitleText(const FText& InText) const { if (ConfirmationTitle) ConfirmationTitle->SetText(InText); }

void UConfirmationPopupUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ConfirmationTitle) ConfirmationTitle->SetText(ConfirmationTitleText);
}

void UConfirmationPopupUI::NativeConstruct()
{
	Super::NativeConstruct();
	AcceptButton->SetFontSize(ButtonsFontSize);
	DeclineButton->SetFontSize(ButtonsFontSize);
	FadeOutDelegate.BindDynamic(this, &UConfirmationPopupUI::FadeOutAnimFinished);
}

void UConfirmationPopupUI::FadeOutAnimFinished()
{
	UnbindAllFromAnimationFinished(PopupFadeIn);
	SetVisibility(ESlateVisibility::Collapsed);
}
