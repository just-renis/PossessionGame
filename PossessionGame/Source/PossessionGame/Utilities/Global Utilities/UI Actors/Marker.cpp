#include "Marker.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/UI/SecondaryWidgets/ObjectiveMarker.h"

void AMarker::Toggle(const bool bState)
{
	if (bState) GetWorld()->GetTimerManager().SetTimer(MarkerUpdateHandle, this, &AMarker::UpdateMarkerText, 0.05f, true);
	else GetWorld()->GetTimerManager().ClearTimer(MarkerUpdateHandle);
	SetActorHiddenInGame(!bState);
}

void AMarker::DestroyThis()
{
	Toggle(false);
	Destroy();
}

AMarker::AMarker()
{
	MarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("markerWidgetComponent");
	RootComponent = MarkerWidgetComponent;
}

void AMarker::BeginPlay()
{
	Super::BeginPlay();
	Marker = Cast<UObjectiveMarker>(MarkerWidgetComponent->GetWidget());
	PlayerController = GetWorld()->GetFirstPlayerController();
	GetWorld()->GetTimerManager().SetTimer(MarkerUpdateHandle, this, &AMarker::UpdateMarkerText, 0.05f, true);
}

void AMarker::UpdateMarkerText() const
{
	if (!PlayerController) return;
	Marker->UpdateMarkerText(FMath::Sqrt(FVector::DistSquared(GetActorLocation(), PlayerController->GetPawn()->GetActorLocation())) * 0.01f);
}
