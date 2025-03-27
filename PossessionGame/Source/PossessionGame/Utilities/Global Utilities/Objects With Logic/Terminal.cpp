#include "Terminal.h"
#include "Components/WidgetComponent.h"

ATerminal::ATerminal()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("terminalMesh"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	RootComponent = Mesh;
	InteractableWidget->SetupAttachment(RootComponent);
}
