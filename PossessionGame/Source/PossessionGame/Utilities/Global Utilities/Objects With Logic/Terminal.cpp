#include "Terminal.h"
#include "PoolObject.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EGameMode.h"
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
	DelegateManager->OnGameModeInitiator.AddUObject(this, &ATerminal::EnableTerminal);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATerminal::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATerminal::DisableInteractionWidget);
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
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

void ATerminal::ChangeOutlineState(const bool bState)
{
	Mesh->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) UKismetSystemLibrary::K2_SetTimer(this, "DisableOutline", 0.245f, false);
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
