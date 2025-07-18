#include "VariationsComputer.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/UI/MainWidgets/VariationsUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

AVariationsComputer::AVariationsComputer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AVariationsComputer::Interact()
{
	if (!GetIsInteractable()) return;
	if (ComputerType == EWidget::VariationsUI) GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI, 1)->OpenUI();
	else GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI, 1)->OpenUI();
}
