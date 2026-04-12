#include "MarkerManager.h"
#include "PossessionGame/Enums/EMarkerType.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/Marker.h"

int32 UMarkerManager::SpawnMarkerAtLocation(const FVector& InLocation, const bool bSaveMarker, const EMarkerType InMarkerType)
{
	TObjectPtr<AMarker> Marker;
	if (const TObjectPtr<UTexture2D>* MarkerTexture = MarkerTypes.Find(InMarkerType))
	{
		Marker = GetWorld()->SpawnActor<AMarker>(MarkerClass, InLocation, FRotator::ZeroRotator);
		Marker->SetImage(*MarkerTexture);
	}
	else Marker = GetWorld()->SpawnActor<AMarker>(MarkerClass, InLocation, FRotator::ZeroRotator);
	Marker->SaveMarker(++NextId, bSaveMarker);
	return NextId;
}

void UMarkerManager::Init(const TSubclassOf<AMarker>& InMarkerClass, const TMap<EMarkerType, TObjectPtr<UTexture2D>>& InMarkersTypes)
{
	MarkerClass = InMarkerClass;
	MarkerTypes = InMarkersTypes;
}

void UMarkerManager::DestroyMarker(const int32 InId) const { TriggerDestroyMarkerWithId.Broadcast(InId); }

void UMarkerManager::DestroyMarkers() const { TriggerDestroyMarkers.Broadcast(); }
