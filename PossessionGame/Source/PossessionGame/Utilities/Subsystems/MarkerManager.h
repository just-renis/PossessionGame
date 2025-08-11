#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EMarkerType.h"
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
	
	void SpawnMarkerAtLocation(const FVector& InLocation, const EMarkerType InMarkerType = EMarkerType::Diamond) const;
	void Init(const TSubclassOf<AMarker>& InMarkerClass, const TMap<EMarkerType, TObjectPtr<UTexture2D>>& InMarkersTypes = TMap<EMarkerType, TObjectPtr<UTexture2D>>());
	void DestroyMarkers();
private:
	FTriggerDestroyMarkers TriggerDestroyMarkers;
	FOnMarkerTriggered OnMarkerTriggered;
	UPROPERTY()
	TSubclassOf<AMarker> MarkerClass;
	UPROPERTY()
	TMap<EMarkerType, TObjectPtr<UTexture2D>> MarkerTypes;
};
