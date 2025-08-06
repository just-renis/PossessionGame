#include "ExplanationHighlight.h"
#include "Components/Image.h"

void UExplanationHighlight::SetTexture(const TObjectPtr<UTexture2D>& InTexture) const
{
	if (ExplanationHighlight) ExplanationHighlight->SetBrushFromAtlasInterface(InTexture);
}
