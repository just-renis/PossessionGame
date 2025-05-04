#include "Terminal.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

ATerminal::ATerminal()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
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
	if (Marker) GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::ObjectiveMarker, Marker);
	DelegateManager->OnTerminalInteraction.RemoveAll(this);
	DelegateManager->OnTerminalHackFinish.RemoveAll(this);
	DisableTerminal();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalInteraction.Broadcast();
}

void ATerminal::ShowMarker()
{
	if (!Marker) Marker = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::ObjectiveMarker, GetActorLocation() + FVector(0.f, 0.f, 140.f));
	Marker->Init();
}

void ATerminal::EnableTerminal(const EGameMode InGameMode)
{
	if (InGameMode == EGameMode::HackTerminalsMode)
	{
		ToggleInteractableWidget(true);
		SetIsInteractable(true);
	}
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
		if (Marker) Marker->Disable();
		if (InteractableWidget) InteractableWidget->SetVisibility(false);
	}
}
