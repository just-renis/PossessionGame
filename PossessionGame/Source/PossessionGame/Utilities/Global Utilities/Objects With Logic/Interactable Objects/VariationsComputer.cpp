#include "VariationsComputer.h"
#include "PossessionGame/UI/MainWidgets/AbilitiesMenuUI.h"
#include "PossessionGame/UI/MainWidgets/VariationsUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

AVariationsComputer::AVariationsComputer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AVariationsComputer::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitialized.AddUObject(this, &AVariationsComputer::SpawnMarker);
}

void AVariationsComputer::Interact()
{
	if (!GetIsInteractable()) return;
	if (ComputerType == EWidget::VariationsUI) GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI)->OpenUI();
	else GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UAbilitiesMenuUI>(EWidget::AbilitiesMenuUI)->OpenUI();
}

void AVariationsComputer::SpawnMarker() const
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitialized.RemoveAll(this);
	GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * MarkerUpwardsUnits, EMarkerType::Question);
}
