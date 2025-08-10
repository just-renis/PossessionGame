#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MarkerManager.generated.h"

class AMarkerTrigger;
class AMarker;

DECLARE_DELEGATE_OneParam(FOnMarkerTriggered, const int32);
DECLARE_MULTICAST_DELEGATE(FTriggerDestroyMarkers);

UCLASS()
class POSSESSIONGAME_API UMarkerManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	FOnMarkerTriggered& GetOnMarkerTriggeredDelegate() { return OnMarkerTriggered; }
	FTriggerDestroyMarkers& GetTriggerDestroyMarkersDelegate() { return TriggerDestroyMarkers; }
	
	void Init(const TSubclassOf<AMarker>& InMarkerClass);
	void SpawnMarkerAtLocation(const FVector& InLocation) const;
	void SpawnMarkerAtMarkerTrigger(const int32 InIndex);
	void DestroyMarkers();
private:
	FOnMarkerTriggered OnMarkerTriggered;
	FTriggerDestroyMarkers TriggerDestroyMarkers;
	UPROPERTY()
	TSubclassOf<AMarker> MarkerClass;
	UPROPERTY()
	TArray<TObjectPtr<AMarkerTrigger>> MarkerTriggers;

	void OnMarkerTriggerPassed(const int32 InNextMarkerToTrigger);
};
