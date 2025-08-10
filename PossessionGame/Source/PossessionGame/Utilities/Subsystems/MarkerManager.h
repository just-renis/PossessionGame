#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MarkerManager.generated.h"

class AMarkerTrigger;
class AMarker;

DECLARE_MULTICAST_DELEGATE(FTriggerDestroyMarkers);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMarkerTriggered, const int32);

UCLASS()
class POSSESSIONGAME_API UMarkerManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	FTriggerDestroyMarkers& GetTriggerDestroyMarkersDelegate() { return TriggerDestroyMarkers; }
	FOnMarkerTriggered& GetOnMarkerTriggeredDelegate() { return OnMarkerTriggered; }
	
	void Init(const TSubclassOf<AMarker>& InMarkerClass);
	void SpawnMarkerAtLocation(const FVector& InLocation) const;
	void DestroyMarkers();
private:
	FTriggerDestroyMarkers TriggerDestroyMarkers;
	FOnMarkerTriggered OnMarkerTriggered;
	UPROPERTY()
	TSubclassOf<AMarker> MarkerClass;
};
