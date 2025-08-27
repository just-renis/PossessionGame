#include "UsbTerminal.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AUsbTerminal::AUsbTerminal()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AUsbTerminal::BeginPlay()
{
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Super::BeginPlay();
	DelegateManager->OnUsbCollected.BindUObject(this, &AUsbTerminal::EnableInteraction);
	DelegateManager->ResetChargeTerminal.BindUObject(this, &AUsbTerminal::ResetTerminal);
	DisableInteraction();
}

void AUsbTerminal::Interact()
{
	if (!GetIsInteractable() || !bIsUsbCollected) return;
	if (UsbInsertedAmount < UsbInsertableAmount)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UsbInsert);
		(void)DelegateManager->OnUsbInserted.ExecuteIfBound();
		if (++UsbInsertedAmount < UsbInsertableAmount) GetWorld()->GetSubsystem<UEnemyCommunicator>()->SetIsUsbDroppable(true);
	}
	DisableInteraction();
}

void AUsbTerminal::ShowMarker() const { GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * 140.f); }

void AUsbTerminal::EnableInteraction()
{
	ShowMarker();
	bIsUsbCollected = true;
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AUsbTerminal::DisableInteraction()
{
	GetWorld()->GetSubsystem<UMarkerManager>()->DestroyMarkers();
	bIsUsbCollected = false;
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AUsbTerminal::ResetTerminal()
{
	UsbInsertedAmount = 0;
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->SetIsUsbDroppable(true);
}
