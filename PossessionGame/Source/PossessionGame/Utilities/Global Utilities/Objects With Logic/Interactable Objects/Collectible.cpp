#include "Collectible.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/CollectibleData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/UI/SecondaryWidgets/InteractableUI.h"
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
	if (Data->HasAura()) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(Data->GetAuraName(), nullptr, &Aura, Collision, NAME_None);
	PoolingManager = GetWorld()->GetSubsystem<UPoolingManager>();
	InteractableUI = Cast<UInteractableUI>(InteractableWidget->GetWidget());
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACollectible::DisableInteractionWidget);
}

void ACollectible::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FloatHandle);
	GetWorldTimerManager().ClearTimer(SpinHandle);
	GetWorldTimerManager().ClearTimer(DisableOutlineHandle);
	GetWorldTimerManager().ClearTimer(FadeOutAnimHandle);
	Super::EndPlay(EndPlayReason);
}

void ACollectible::Init(bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	if (Aura) Aura->Activate();
	if (Data->GetIsFloating())
	{
		InitialSpawnZ = GetActorLocation().Z;
		GetWorldTimerManager().SetTimer(FloatHandle, this, &ACollectible::Float, 0.025f, true);
	}
	if (Data->GetIsSpinning()) GetWorldTimerManager().SetTimer(SpinHandle, this, &ACollectible::Spin, 0.025f, true);
	SetActorRotation(Data->GetRotation());
	EnableCollectible();
}

void ACollectible::Interact()
{
	if (!GetIsInteractable()) return;
	if (Aura) Aura->Deactivate();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UsbCollect);
	GetWorldTimerManager().ClearTimer(FloatHandle);
	GetWorldTimerManager().ClearTimer(SpinHandle);
	PoolingManager->ReturnObjectToPool(Data->GetCollectibleName(), this);
	DisableCollectible();
	switch (Data->GetCollectibleName())
	{
		case EPoolObject::Usb: (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnUsbCollected.ExecuteIfBound(); break;
		case EPoolObject::Artifact: (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnArtifactCollected.ExecuteIfBound(); break;
		case EPoolObject::Coin: (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnCoinCollected.ExecuteIfBound(1);
		default: ;
	}
}

void ACollectible::ShowMarker()
{
	if (!Marker) Marker = GetWorld()->SpawnActor<AMarker>(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetMarkerClass(), GetActorLocation() + FVector(0.f, 0.f, 50.f), FRotator::ZeroRotator);
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
	if (bState) GetWorldTimerManager().SetTimer(DisableOutlineHandle, this, &ACollectible::DisableOutline, 0.245f, false);
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
	NewRotation.Yaw += Data->GetSpinningSpeed() * 0.025f;
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
