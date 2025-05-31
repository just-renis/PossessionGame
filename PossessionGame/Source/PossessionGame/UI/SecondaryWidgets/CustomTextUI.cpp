#include "CustomTextUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void UCustomTextUI::SetText(const FText& InText) const { if (CustomText) CustomText->SetText(InText); }

void UCustomTextUI::PlayFadeInAnim(const bool bState)
{
	if (bState) GetWorld()->GetTimerManager().SetTimer(FadeInHandle, this, &UCustomTextUI::FadeInAnimFinished, FadeInAnim->GetEndTime(), false);
	PlayAnimation(FadeInAnim, 0, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UCustomTextUI::FadeInAnimFinished() { PlayFadeInAnim(false); }
