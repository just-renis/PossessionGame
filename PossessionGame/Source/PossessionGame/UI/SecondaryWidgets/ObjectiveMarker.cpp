#include "ObjectiveMarker.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UObjectiveMarker::Init(const bool bIsTextNeeded) const { if (!bIsTextNeeded && MarkerText) MarkerText->SetVisibility(ESlateVisibility::Collapsed); }

void UObjectiveMarker::UpdateMarkerText(const int32 InMeters) const
{
	if (!MarkerText) return;
	MarkerText->SetText(FText::FromString(FString::FormatAsNumber(InMeters) + "m"));
}

void UObjectiveMarker::SetMarkerImage(const TObjectPtr<UTexture2D>& InTexture) const
{
	if (!MarkerImage) return;
	MarkerImage->SetBrushFromAtlasInterface(InTexture);
}
