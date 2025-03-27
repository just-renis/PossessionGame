#include "Interactable.h"
#include "Components/WidgetComponent.h"

AInteractable::AInteractable()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidget"));
	RootComponent = Mesh;
	InteractableWidget->SetupAttachment(RootComponent);
}
