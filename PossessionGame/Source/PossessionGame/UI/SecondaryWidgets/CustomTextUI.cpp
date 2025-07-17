#include "CustomTextUI.h"
#include "Components/TextBlock.h"

void UCustomTextUI::SetText(const FText& InText) const { if (CustomText) CustomText->SetText(InText); }

void UCustomTextUI::PlayFadeInAnim(const bool bState)
{
	PlayAnimation(FadeInAnim, 0, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}
