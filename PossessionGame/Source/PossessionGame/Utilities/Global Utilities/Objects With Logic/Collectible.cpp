#include "Collectible.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

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
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachOrLoadVfx(EVfx::UsbAura, &UsbAura, Collision, NAME_None);
	PoolingManager = GetWorld()->GetSubsystem<UPoolingManager>();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACollectible::DisableInteractionWidget);
}

void ACollectible::Init(bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	if (UsbAura) UsbAura->Activate();
	InitialSpawnZ = GetActorLocation().Z;
	UKismetSystemLibrary::K2_SetTimer(this, "Float", 0.05f, true);
	SetActorRotation(FRotator(0.f, 0.f, 45.f));
	EnableCollectible();
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
}

void ACollectible::Interact()
{
	if (!GetIsInteractable()) return;
	if (UsbAura) UsbAura->Deactivate();
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UsbCollect);
	UKismetSystemLibrary::K2_ClearTimer(this, "Float");
	PoolingManager->ReturnObjectToPool(EPoolObject::Collectible, this);
	DisableCollectible();
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnUsbCollected.ExecuteIfBound();
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

void ACollectible::ChangeOutlineState(const bool bState)
{
	Mesh->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) UKismetSystemLibrary::K2_SetTimer(this, "DisableOutline", 0.245f, false);
}

void ACollectible::Float()
{
	FVector NewLocation = GetActorLocation();
	FRotator NewRotation = GetActorRotation();
	const float FloatValue = FMath::Sin(GetWorld()->GetTimeSeconds() * 2.f) * 20.f;
	NewLocation.Z = InitialSpawnZ + FloatValue;
	NewRotation.Yaw += 3.f;
	SetActorLocationAndRotation(NewLocation, NewRotation);
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
