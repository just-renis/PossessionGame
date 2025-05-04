#include "InteractableObject.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AInteractableObject::AInteractableObject()
{
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	InteractableWidget->SetupAttachment(RootComponent);
	Collision->SetupAttachment(RootComponent);
}

void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AInteractableObject::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AInteractableObject::DisableInteractionWidget);
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
}

void AInteractableObject::ChangeOutlineState(const bool bState)
{
	if (bIsSkeletalMesh) GetSkeletalMesh()->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	else GetMesh()->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) UKismetSystemLibrary::K2_SetTimer(this, "DisableOutline", 0.245f, false);
}

void AInteractableObject::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player")) InteractableWidget->SetVisibility(true);
}

void AInteractableObject::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player")) InteractableWidget->SetVisibility(false);
}
