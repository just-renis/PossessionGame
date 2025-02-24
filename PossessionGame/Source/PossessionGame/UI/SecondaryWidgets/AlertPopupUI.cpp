#include "AlertPopupUI.h"
#include "Components/TextBlock.h"

void UAlertPopupUI::PlayPopupFadeInAnim(bool bState)
{
	bState ? PlayAnimation(PopupFadeIn, GetAnimationCurrentTime(PopupFadeIn))
	: PlayAnimation(PopupFadeIn, GetAnimationCurrentTime(PopupFadeIn), 1, EUMGSequencePlayMode::Reverse);
}

void UAlertPopupUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (AlertTitle) AlertTitle->SetText(AlertTitleText);
	if (AlertButtonText) AlertButtonText->SetText(AlertText);
}