#include "Terminal.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/Marker.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ATerminal::ATerminal()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void ATerminal::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager->OnHackModeStart.AddUObject(this, &ATerminal::ShowMarker);
	DelegateManager->OnTerminalInteraction.AddUObject(this, &ATerminal::DisableTerminal);
	DelegateManager->OnTerminalHackFinish.AddUObject(this, &ATerminal::EnableTerminal);
	DelegateManager->OnGameModeInitiator.AddUObject(this, &ATerminal::EnableTerminal);
}

void ATerminal::Interact()
{
	if (!GetIsInteractable()) return;
	if (Marker) Marker->DestroyThis();
	DelegateManager->OnTerminalInteraction.RemoveAll(this);
	DelegateManager->OnTerminalHackFinish.RemoveAll(this);
	DelegateManager->OnGameModeInitiator.RemoveAll(this);
	DisableTerminal();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalInteraction.Broadcast();
}

void ATerminal::ShowMarker()
{
	if (!Marker) Marker = GetWorld()->SpawnActor<AMarker>(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetMarkerClass(), GetActorLocation() + FVector(0.f, 0.f, 140.f), FRotator::ZeroRotator);
	Marker->Toggle(true);
}

void ATerminal::EnableTerminal(const EGameMode InGameMode)
{
	if (InGameMode == EGameMode::HackTerminalsMode) ToggleInteractableWidget(true);
}

void ATerminal::DisableTerminal()
{
	ToggleInteractableWidget(false);
	SetIsInteractable(false);
}

void ATerminal::ToggleInteractableWidget(const bool bState)
{
	if (bState)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ShowMarker();
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (Marker) Marker->Toggle(false);
		FadeOutInteractableUI();
	}
}
