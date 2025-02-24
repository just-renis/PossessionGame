#include "ConfirmationPopupUI.h"
#include "Components/TextBlock.h"

void UConfirmationPopupUI::PlayPopupFadeInAnim(bool bState)
{
	bState ? PlayAnimation(PopupFadeIn, GetAnimationCurrentTime(PopupFadeIn), 1, EUMGSequencePlayMode::Reverse)
	: PlayAnimation(PopupFadeIn, GetAnimationCurrentTime(PopupFadeIn));
}

void UConfirmationPopupUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ConfirmationTitle) ConfirmationTitle->SetText(ConfirmationTitleText);
	if (AcceptButtonText) AcceptButtonText->SetText(AcceptText);
	if (DeclineButtonText) DeclineButtonText->SetText(DeclineText);
}
