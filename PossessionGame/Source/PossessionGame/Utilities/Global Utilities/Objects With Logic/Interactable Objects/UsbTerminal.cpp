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
	for (int i = 0; i < 5; i++) UsbMeshes[i] = CreateDefaultSubobject<UStaticMeshComponent>(FName("UsbMesh" + FString::FromInt(i)));
}

void AUsbTerminal::BeginPlay()
{
	Super::BeginPlay();
	for (int i = 0; i < 5; i++)
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
	DelegateManager->OnUsbCollected.BindUObject(this, &AUsbTerminal::EnableInteraction);
	DisableInteraction();
}

void AUsbTerminal::Interact()
{
	if (!GetIsInteractable()) return;
	if (CurrentUsbIndex < 5)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UsbInsert);
		UsbMeshes[CurrentUsbIndex]->SetVisibility(true);
		(void)DelegateManager->OnUsbInserted.ExecuteIfBound();
		if (++CurrentUsbIndex < 5) GetWorld()->GetSubsystem<UEnemyCommunicator>()->SetIsUsbDroppable(true);
	}
	DisableInteraction();
}

void AUsbTerminal::ShowMarker() const { GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * 140.f); }

void AUsbTerminal::EnableInteraction()
{
	ToggleInteractableWidget(true);
	SetIsInteractable(true);
}

void AUsbTerminal::DisableInteraction()
{
	ToggleInteractableWidget(false);
	SetIsInteractable(false);
}

void AUsbTerminal::ToggleInteractableWidget(const bool bState)
{
	if (bState)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ShowMarker();
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetWorld()->GetSubsystem<UMarkerManager>()->DestroyMarkers();
		FadeOutInteractableUI();
	}
}
