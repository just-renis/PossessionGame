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
	for (int i = 0; i < 4; i++) UsbMeshes[i] = CreateDefaultSubobject<UStaticMeshComponent>(FName("UsbMesh" + FString::FromInt(i)));
}

void AUsbTerminal::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	for (int i = 0; i < 4; i++)
	{
		if (UsbMeshes[i])
		{
			UsbMeshes[i]->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Socket" + FString::FromInt(i + 1)));
			UsbMeshes[i]->SetStaticMesh(UsbMesh);
			UsbMeshes[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			UsbMeshes[i]->SetGenerateOverlapEvents(false);
			UsbMeshes[i]->CanCharacterStepUpOn = ECB_No;
			UsbMeshes[i]->SetVisibility(false);
		}
	}
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AUsbTerminal::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AUsbTerminal::DisableInteractionWidget);
	DelegateManager->OnUsbCollected.BindUObject(this, &AUsbTerminal::EnableInteraction);
	DisableInteraction();
}

void AUsbTerminal::Interact()
{
	if (!GetIsInteractable()) return;
	if (CurrentUsbIndex < 4)
	{
		UsbMeshes[CurrentUsbIndex]->SetVisibility(true);
		(void)DelegateManager->OnUsbInserted.ExecuteIfBound();
		if (++CurrentUsbIndex < 4) DelegateManager->SetIsUsbDroppable(true);
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
