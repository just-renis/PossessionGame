#include "Marker.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/UI/SecondaryWidgets/ObjectiveMarker.h"

AMarker::AMarker()
{
	MarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("markerWidgetComponent");
	RootComponent = MarkerWidgetComponent;
}

void AMarker::BeginPlay()
{
	Super::BeginPlay();
	Marker = Cast<UObjectiveMarker>(MarkerWidgetComponent->GetWidget());
}

void AMarker::Init(bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	PlayerController = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateMarkerText", 0.05f, true);
}

void AMarker::Disable(const bool bHide)
{
	Super::Disable(bHide);
	UKismetSystemLibrary::K2_ClearTimer(this, "UpdateMarkerText");
}

void AMarker::UpdateMarkerText() const
{
	if (!PlayerController) return;
	Marker->UpdateMarkerText(FMath::Sqrt(FVector::DistSquared(GetActorLocation(), PlayerController->GetPawn()->GetActorLocation())) / 100.f);
}
