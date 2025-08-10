#include "MarkerManager.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/MarkerTrigger.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/Marker.h"

void UMarkerManager::Init(const TSubclassOf<AMarker>& InMarkerClass)
{
	MarkerClass = InMarkerClass;
	GetOnMarkerTriggeredDelegate().BindUObject(this, &UMarkerManager::OnMarkerTriggerPassed);
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMarkerTrigger::StaticClass(), Actors);
	for (TObjectPtr<AActor> Actor : Actors) MarkerTriggers.Add(Cast<AMarkerTrigger>(Actor));
}

void UMarkerManager::SpawnMarkerAtLocation(const FVector& InLocation) const
{
	GetWorld()->SpawnActor<AMarker>(MarkerClass, InLocation, FRotator::ZeroRotator);
}

void UMarkerManager::SpawnMarkerAtMarkerTrigger(const int32 InIndex)
{
	for (int32 i = 0; i < MarkerTriggers.Num(); i++)
	{
		if (MarkerTriggers[i]->GetMarkerTriggerIndex() != InIndex) continue;
		SpawnMarkerAtLocation(MarkerTriggers[i]->GetActorLocation());
		return;
	}
}

void UMarkerManager::DestroyMarkers() { GetTriggerDestroyMarkersDelegate().Broadcast(); }

void UMarkerManager::OnMarkerTriggerPassed(const int32 InNextMarkerToTrigger)
{
	DestroyMarkers();
	SpawnMarkerAtMarkerTrigger(InNextMarkerToTrigger);
}
