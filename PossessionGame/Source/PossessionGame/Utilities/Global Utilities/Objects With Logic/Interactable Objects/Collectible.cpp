#include "Collectible.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Data Assets/Base Data Assets/CollectibleData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/UI/SecondaryWidgets/InteractableUI.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraVariationData.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/Marker.h"
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
	InteractableWidget->SetVisibility(false);
}

void ACollectible::BeginPlay()
{
	Super::BeginPlay();
	if (Data->HasAura()) GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(Data->GetAuraName(), nullptr, FNiagaraVariationData(), &Aura, Collision, NAME_None);
	PoolingManager = GetWorld()->GetSubsystem<UPoolingManager>();
	InteractableUI = Cast<UInteractableUI>(InteractableWidget->GetWidget());
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACollectible::DisableInteractionWidget);
}

void ACollectible::Init(bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	if (Aura) Aura->Activate();
	if (Data->GetIsFloating())
	{
		InitialSpawnZ = GetActorLocation().Z;
		UKismetSystemLibrary::K2_SetTimer(this, "Float", 0.025f, true);
	}
	if (Data->GetIsSpinning()) UKismetSystemLibrary::K2_SetTimer(this, "Spin", 0.025f, true);
	SetActorRotation(Data->GetRotation());
	EnableCollectible();
}

void ACollectible::Interact()
{
	if (!GetIsInteractable()) return;
	if (Aura) Aura->Deactivate();
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UsbCollect);
	UKismetSystemLibrary::K2_ClearTimer(this, "Float");
	UKismetSystemLibrary::K2_ClearTimer(this, "Spin");
	PoolingManager->ReturnObjectToPool(Data->GetCollectibleName(), this);
	DisableCollectible();
	if (Data->GetCollectibleName() == EPoolObject::Usb) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnUsbCollected.ExecuteIfBound();
	else if (Data->GetCollectibleName() == EPoolObject::Artifact) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnArtifactCollected.ExecuteIfBound();
}

void ACollectible::ShowMarker()
{
	if (!Marker) Marker = GetWorld()->SpawnActor<AMarker>(GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->GetMarkerClass(), GetActorLocation() + FVector(0.f, 0.f, 50.f), FRotator::ZeroRotator);
	Marker->Toggle(true);
	Marker->SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, 50.f));
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
		if (bMarkerIncluded && Data->GetIsMarkerEnabled()) ShowMarker();
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (bMarkerIncluded && Marker && Data->GetIsMarkerEnabled()) Marker->Toggle(false);
		if (InteractableUI) InteractableUI->PlayFadeInAnim(false);
		GetWorldTimerManager().SetTimer(FadeOutAnimHandle, this, &ACollectible::FadeOutAnimFinished, InteractableUI->GetFadeInAnimLength(), false);
	}
}

void ACollectible::ChangeOutlineState(const bool bState)
{
	Mesh->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) UKismetSystemLibrary::K2_SetTimer(this, "DisableOutline", 0.245f, false);
}

void ACollectible::FadeOutAnimFinished() const { if (InteractableWidget) InteractableWidget->SetVisibility(false); }

void ACollectible::Float()
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = InitialSpawnZ + FMath::Sin(GetWorld()->GetTimeSeconds() * 2.f) * 20.f;
	SetActorLocation(NewLocation);
}

void ACollectible::Spin()
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += 1.5f;
	SetActorRotation(NewRotation);
}

void ACollectible::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	if (InteractableWidget) InteractableWidget->SetVisibility(true);
	if (InteractableUI) InteractableUI->PlayFadeInAnim(true);
}

void ACollectible::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag("Player") || !InteractableUI) return;
	InteractableUI->PlayFadeInAnim(false);
	GetWorldTimerManager().SetTimer(FadeOutAnimHandle, this, &ACollectible::FadeOutAnimFinished, InteractableUI->GetFadeInAnimLength(), false);
}
