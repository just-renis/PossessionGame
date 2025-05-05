#include "VariationsComputer.h"
#include "PossessionGame/UI/MainWidgets/VariationsUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

AVariationsComputer::AVariationsComputer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
}

void AVariationsComputer::Interact()
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI)->OpenUI();
	UE_LOG(LogTemp, Warning, TEXT("INTERACTED"));
}
