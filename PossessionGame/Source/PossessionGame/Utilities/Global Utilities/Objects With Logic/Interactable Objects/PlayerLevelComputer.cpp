#include "PlayerLevelComputer.h"
#include "PossessionGame/UI/MainWidgets/PlayerLevelSystemUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

APlayerLevelComputer::APlayerLevelComputer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void APlayerLevelComputer::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitialized.AddUObject(this, &APlayerLevelComputer::SpawnMarker);
}

void APlayerLevelComputer::Interact()
{
	if (!GetIsInteractable()) return;
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerLevelSystemUI>(EWidget::PlayerLevelSystemUI)->OpenUI();
	/*if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsExplanationShown(EExplanation::VariationMainUI))
	{
		(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::VariationMainUI, GetGameInstance());
	}*/
}

void APlayerLevelComputer::SpawnMarker() const
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitialized.RemoveAll(this);
	GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * MarkerUpwardsUnits, EMarkerType::RedArrow);
}
