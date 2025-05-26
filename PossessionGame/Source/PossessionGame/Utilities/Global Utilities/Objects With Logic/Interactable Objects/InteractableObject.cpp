#include "InteractableObject.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/UI/SecondaryWidgets/InteractableUI.h"

void AInteractableObject::FadeOutInteractableUI()
{
	if (!InteractableUI) return;
	InteractableUI->PlayFadeInAnim(false);
	GetWorldTimerManager().SetTimer(FadeOutAnimHandle, this, &AInteractableObject::FadeOutAnimFinished, InteractableUI->GetFadeInAnimLength(), false);
}

AInteractableObject::AInteractableObject()
{
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RootComponent = Collision;
	InteractableWidget->SetupAttachment(RootComponent);
	InteractableWidget->SetVisibility(false);
}

void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AInteractableObject::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AInteractableObject::DisableInteractionWidget);
	InteractableUI = Cast<UInteractableUI>(InteractableWidget->GetWidget());
}

void AInteractableObject::ChangeOutlineState(const bool bState)
{
	if (bIsSkeletalMesh) GetSkeletalMesh()->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	else GetMesh()->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) UKismetSystemLibrary::K2_SetTimer(this, "DisableOutline", 0.245f, false);
}

void AInteractableObject::FadeOutAnimFinished() const { if (InteractableWidget)  InteractableWidget->SetVisibility(false); }

void AInteractableObject::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	if (InteractableWidget) InteractableWidget->SetVisibility(true);
	if (InteractableUI) InteractableUI->PlayFadeInAnim(true);
	SetIsInteractable(true);
}

void AInteractableObject::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag("Player") || !InteractableUI) return;
	FadeOutInteractableUI();
	SetIsInteractable(false);
}
