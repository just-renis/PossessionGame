#include "MarkerManager.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/Marker.h"

void UMarkerManager::Init(const TSubclassOf<AMarker>& InMarkerClass) { MarkerClass = InMarkerClass; }

void UMarkerManager::SpawnMarkerAtLocation(const FVector& InLocation) const { GetWorld()->SpawnActor<AMarker>(MarkerClass, InLocation, FRotator::ZeroRotator); }

void UMarkerManager::DestroyMarkers() { GetTriggerDestroyMarkersDelegate().Broadcast(); }
