#include "ExplanationHighlight.h"
#include "Components/Image.h"

void UExplanationHighlight::SetTexture(const TObjectPtr<UTexture2D>& InTexture) const
{
	if (ExplanationHighlight) ExplanationHighlight->SetBrushFromAtlasInterface(InTexture);
}

void UExplanationHighlight::PlayFadeAnim(const bool bState) { PlayAnimation(bState ? FadeInAnim : FadeOutAnim); }
