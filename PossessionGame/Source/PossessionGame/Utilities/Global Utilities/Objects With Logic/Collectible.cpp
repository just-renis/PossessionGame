#include "Collectible.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

ACollectible::ACollectible()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("collectibleMesh"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RootComponent = Mesh;
	Collision->SetupAttachment(RootComponent);
	InteractableWidget->SetupAttachment(RootComponent);
}

void ACollectible::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	PoolingManager = GetWorld()->GetSubsystem<UPoolingManager>();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACollectible::DisableInteractionWidget);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACollectible::Init(bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	DelegateManager->OnUsbCollected.AddUObject(this, &ACollectible::DisableCollectible);
	DelegateManager->OnUsbInserted.AddUObject(this, &ACollectible::EnableCollectible);
	if (DelegateManager->GetIsAllUsbInteractable()) ToggleInteractableWidget(true, true);
	else ToggleInteractableWidget(false, false);
}

void ACollectible::Interact()
{
	if (!GetIsInteractable()) return;
	DisableCollectible();
	PoolingManager->ReturnObjectToPool(EPoolObject::Collectible, this);
	DelegateManager->OnUsbInserted.RemoveAll(this);
	DelegateManager->OnUsbCollected.RemoveAll(this);
	DelegateManager->OnUsbCollected.Broadcast();
	DelegateManager->SetAllUsbInteractable(false);
}

void ACollectible::ShowMarker()
{
	Marker = PoolingManager->SpawnObjectFromPool(EPoolObject::ObjectiveMarker, GetActorLocation() + FVector(0.f, 0.f, 50.f));
	Marker->Init();
}

void ACollectible::EnableCollectible()
{
	ToggleInteractableWidget(true, true);
	SetIsInteractable(true);
}

void ACollectible::DisableCollectible()
{
	ToggleInteractableWidget(false, true);
	SetIsInteractable(false);
}

void ACollectible::ToggleInteractableWidget(const bool bState, const bool bMarkerIncluded)
{
	if (bState)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (bMarkerIncluded) ShowMarker();
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (bMarkerIncluded && Marker) PoolingManager->ReturnObjectToPool(EPoolObject::ObjectiveMarker, Marker);
		if (InteractableWidget) InteractableWidget->SetVisibility(false);
	}
}

void ACollectible::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(true);
	}
}

void ACollectible::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(false);
	}
}
