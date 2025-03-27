#include "ObjectiveMarker.h"
#include "Components/TextBlock.h"

void UObjectiveMarker::UpdateMarkerText(const int32 InMeters) const
{
	if (!MarkerText) return;
	MarkerText->SetText(FText::FromString(FString::FormatAsNumber(InMeters) + "m"));
}
