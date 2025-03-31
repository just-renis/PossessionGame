#include "Terminal.h"
#include "PoolObject.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

ATerminal::ATerminal()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("terminalMesh"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RootComponent = Mesh;
	Collision->SetupAttachment(RootComponent);
	InteractableWidget->SetupAttachment(RootComponent);
}

void ATerminal::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnHackModeStart.AddUObject(this, &ATerminal::ShowMarker);
	DelegateManager->OnTerminalInteraction.AddUObject(this, &ATerminal::DisableTerminal);
	DelegateManager->OnTerminalHackFinish.AddUObject(this, &ATerminal::EnableTerminal);
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	ToggleInteractableWidget(true);
}

void ATerminal::Interact()
{
	if (!GetIsInteractable()) return;
	DelegateManager->OnTerminalInteraction.RemoveAll(this);
	DelegateManager->OnTerminalHackFinish.RemoveAll(this);
	ToggleInteractableWidget(false, true);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalInteraction.Broadcast();
	SetIsInteractable(false);
}

void ATerminal::ShowMarker()
{
	Marker = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::ObjectiveMarker, GetActorLocation() + FVector(0.f, 0.f, 140.f));
	Marker->Init();
}

void ATerminal::EnableTerminal()
{
	ToggleInteractableWidget(true, true);
	SetIsInteractable(true);
}

void ATerminal::DisableTerminal()
{
	ToggleInteractableWidget(false, true);
	SetIsInteractable(false);
}

void ATerminal::ToggleInteractableWidget(const bool bState, const bool bMarkerIncluded)
{
	if (bState)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ATerminal::EnableInteractionWidget);
		Collision->OnComponentEndOverlap.AddDynamic(this, &ATerminal::DisableInteractionWidget);
		if (bMarkerIncluded) ShowMarker();
	}
	else
	{
		Collision->OnComponentBeginOverlap.RemoveAll(this);
		Collision->OnComponentEndOverlap.RemoveAll(this);
		if (bMarkerIncluded) GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::ObjectiveMarker, Marker);
		if (InteractableWidget) InteractableWidget->SetVisibility(false);
	}
}

void ATerminal::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(true);
	}
}

void ATerminal::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(false);
	}
}
