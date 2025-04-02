#include "UsbTerminal.h"

#include "Collectible.h"
#include "PoolObject.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

AUsbTerminal::AUsbTerminal()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("collectibleMesh"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RootComponent = Mesh;
	Collision->SetupAttachment(RootComponent);
	InteractableWidget->SetupAttachment(RootComponent);
}

void AUsbTerminal::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnUsbCollected.AddUObject(this, &AUsbTerminal::EnableInteraction);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AUsbTerminal::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AUsbTerminal::DisableInteractionWidget);
	DisableInteraction();
}

void AUsbTerminal::Interact()
{
	if (!GetIsInteractable()) return;
	GetWorld()->SpawnActor<ACollectible>(UsbClass)->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Socket" + FString::FromInt(++CurrentUsbIndex)));
	if (CurrentUsbIndex < 5)
	{
		DelegateManager->OnUsbInserted.Broadcast();
		DelegateManager->SetAllUsbInteractable(true);
	}
	DisableInteraction();
}

void AUsbTerminal::ShowMarker()
{
	Marker = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::ObjectiveMarker, GetActorLocation() + FVector(0.f, 0.f, 50.f));
	Marker->Init();
}

void AUsbTerminal::EnableInteraction()
{
	ToggleInteractableWidget(true, true);
	SetIsInteractable(true);
}

void AUsbTerminal::DisableInteraction()
{
	ToggleInteractableWidget(false, true);
	SetIsInteractable(false);
}

void AUsbTerminal::ToggleInteractableWidget(const bool bState, const bool bMarkerIncluded)
{
	if (bState)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (bMarkerIncluded) ShowMarker();
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (bMarkerIncluded && Marker) GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::ObjectiveMarker, Marker);
		if (InteractableWidget) InteractableWidget->SetVisibility(false);
	}
}

void AUsbTerminal::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(true);
	}
}

void AUsbTerminal::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(false);
	}
}
