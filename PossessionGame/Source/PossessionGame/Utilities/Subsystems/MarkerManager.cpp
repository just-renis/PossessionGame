#include "MarkerManager.h"
#include "PossessionGame/Enums/EMarkerType.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/Marker.h"

void UMarkerManager::SpawnMarkerAtLocation(const FVector& InLocation, const EMarkerType InMarkerType) const
{
	if (const TObjectPtr<UTexture2D> MarkerTexture = *MarkerTypes.Find(InMarkerType))
	{
		GetWorld()->SpawnActor<AMarker>(MarkerClass, InLocation, FRotator::ZeroRotator)->SetImage(MarkerTexture);
	}
	else GetWorld()->SpawnActor<AMarker>(MarkerClass, InLocation, FRotator::ZeroRotator);
}

void UMarkerManager::Init(const TSubclassOf<AMarker>& InMarkerClass, const TMap<EMarkerType, TObjectPtr<UTexture2D>>& InMarkersTypes)
{
	MarkerClass = InMarkerClass;
	MarkerTypes = InMarkersTypes;
}

void UMarkerManager::DestroyMarkers() { GetTriggerDestroyMarkersDelegate().Broadcast(); }
