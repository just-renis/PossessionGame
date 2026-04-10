#include "UsbTerminal.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
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
	DelegateManager->OnUsbInserted.AddUObject(this, &AUsbTerminal::DisableInteraction);
	DelegateManager->OnUsbCollected.AddUObject(this, &AUsbTerminal::EnableInteraction);
	DisableInteraction();
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		if (GetMesh()->GetMaterial(i)->GetName().Contains("Locks")) MID_Locks = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
	}
}

void AUsbTerminal::Interact()
{
	if (!GetIsInteractable() || !bIsUsbCollected) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UsbInsert);
	DelegateManager->OnUsbInserted.Broadcast();
	DelegateManager->OnUsbInserted.RemoveAll(this);
	DelegateManager->OnUsbCollected.RemoveAll(this);
	if (bIsWiredTerminal) DelegateManager->OnUsbInsertionForWires.Broadcast(WireId);
	DisableInteraction();
	if (!MID_Locks) return;
	MID_Locks->SetScalarParameterValue(FName("ColorLockSwitch"), 1.f);
	MID_Locks->SetScalarParameterValue(FName("LightLockSwitch"), 1.f);
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
