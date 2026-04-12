#include "VariationsComputer.h"
#include "PossessionGame/Enums/EExplanation.h"
#include "PossessionGame/UI/MainWidgets/VariationsUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
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
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UVariationsUI>(EWidget::VariationsUI)->OpenUI();
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsExplanationShown(EExplanation::VariationMainUI))
	{
		(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::VariationMainUI, GetGameInstance());
	}
}

void AVariationsComputer::SpawnMarker() const
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitialized.RemoveAll(this);
	GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * MarkerUpwardsUnits, false, EMarkerType::Question);
}
