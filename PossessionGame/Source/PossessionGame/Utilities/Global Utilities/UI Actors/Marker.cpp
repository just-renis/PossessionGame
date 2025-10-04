#include "Marker.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/UI/SecondaryWidgets/ObjectiveMarker.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"

void AMarker::DestroyThis()
{
	GetWorld()->GetSubsystem<UMarkerManager>()->GetTriggerDestroyMarkersDelegate().RemoveAll(this);
	GetWorld()->GetTimerManager().ClearTimer(MarkerUpdateHandle);
	Destroy();
}

void AMarker::SetImage(const TObjectPtr<UTexture2D>& InMarkerTexture) const { if (Marker) Marker->SetMarkerImage(InMarkerTexture); }

AMarker::AMarker()
{
	MarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("markerWidgetComponent");
	RootComponent = MarkerWidgetComponent;
}

void AMarker::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<UMarkerManager>()->GetTriggerDestroyMarkersDelegate().AddUObject(this, &AMarker::DestroyThis);
	Marker = Cast<UObjectiveMarker>(MarkerWidgetComponent->GetWidget());
	PlayerController = GetWorld()->GetFirstPlayerController();
	GetWorld()->GetTimerManager().SetTimer(MarkerUpdateHandle, this, &AMarker::UpdateMarker, 0.05f, true);
}

void AMarker::UpdateMarker() const
{
	if (!PlayerController || !PlayerController->GetPawn() || !Marker) return;
	Marker->UpdateMarkerText(FMath::Sqrt(FVector::DistSquared(GetActorLocation(), PlayerController->GetPawn()->GetActorLocation())) * 0.01f);
}
