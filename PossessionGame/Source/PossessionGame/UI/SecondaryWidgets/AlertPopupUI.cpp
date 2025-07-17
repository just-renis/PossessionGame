#include "AlertPopupUI.h"
#include "MainMenuButtonUI.h"
#include "Components/TextBlock.h"

void UAlertPopupUI::PlayPopupFadeInAnim(const bool bState)
{
	if (AlertButton) AlertButton->SetButtonVisibility(bState);
	if (bState) SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else BindToAnimationFinished(PopupFadeInAnim, AlertPopupFadeOutDelegate);
	PlayAnimation(PopupFadeInAnim, GetAnimationCurrentTime(PopupFadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UAlertPopupUI::SetText(const FText& InText) const { if (AlertTitle) AlertTitle->SetText(InText); }

void UAlertPopupUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetText(AlertTitleText);
}

void UAlertPopupUI::NativeConstruct()
{
	Super::NativeConstruct();
	AlertPopupFadeOutDelegate.BindDynamic(this, &UAlertPopupUI::AlertPopupFadeOutFinished);
}

void UAlertPopupUI::AlertPopupFadeOutFinished()
{
	UnbindAllFromAnimationFinished(PopupFadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}
