#include "VariationsComputer.h"
#include "PossessionGame/UI/MainWidgets/VariationsUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

AVariationsComputer::AVariationsComputer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AVariationsComputer::Interact()
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI)->OpenUI();
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradePanelFadeIn);
}
