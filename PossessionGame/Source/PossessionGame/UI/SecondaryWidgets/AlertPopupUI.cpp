#include "AlertPopupUI.h"
#include "Components/TextBlock.h"

void UAlertPopupUI::PlayPopupFadeInAnim(const bool bState)
{
	if (!PopupFadeIn) return;
	UE_LOG(LogTemp, Warning, TEXT("PlayPopupFadeInAnim %i %s"), bState, *UEnum::GetValueAsString(GetVisibility()));
	PlayAnimation(PopupFadeIn, GetAnimationCurrentTime(PopupFadeIn), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UAlertPopupUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (AlertTitle) AlertTitle->SetText(AlertTitleText);
	if (AlertButtonText) AlertButtonText->SetText(AlertText);
}