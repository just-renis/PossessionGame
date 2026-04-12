#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EMarkerType.h"
#include "Subsystems/WorldSubsystem.h"
#include "MarkerManager.generated.h"

class AMarkerTrigger;
class AMarker;

DECLARE_MULTICAST_DELEGATE(FTriggerDestroyMarkers);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMarkerTriggered, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FTriggerDestroyMarkerWithId, const int32);

UCLASS()
class POSSESSIONGAME_API UMarkerManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	int32 SpawnMarkerAtLocation(const FVector& InLocation, const bool bSaveMarker, const EMarkerType InMarkerType = EMarkerType::Diamond);
	FTriggerDestroyMarkers& GetTriggerDestroyMarkersDelegate() { return TriggerDestroyMarkers; }
	FOnMarkerTriggered& GetOnMarkerTriggeredDelegate() { return OnMarkerTriggered; }
	FTriggerDestroyMarkerWithId& GetTriggerDestroyMarkerWithId() { return TriggerDestroyMarkerWithId; }
	
	void Init(const TSubclassOf<AMarker>& InMarkerClass, const TMap<EMarkerType, TObjectPtr<UTexture2D>>& InMarkersTypes = TMap<EMarkerType, TObjectPtr<UTexture2D>>());
	void DestroyMarker(const int32 InId) const;
	void DestroyMarkers() const;
private:
	int32 NextId = 0;
	FTriggerDestroyMarkers TriggerDestroyMarkers;
	FOnMarkerTriggered OnMarkerTriggered;
	FTriggerDestroyMarkerWithId TriggerDestroyMarkerWithId;
	UPROPERTY()
	TSubclassOf<AMarker> MarkerClass;
	UPROPERTY()
	TMap<EMarkerType, TObjectPtr<UTexture2D>> MarkerTypes;
};
