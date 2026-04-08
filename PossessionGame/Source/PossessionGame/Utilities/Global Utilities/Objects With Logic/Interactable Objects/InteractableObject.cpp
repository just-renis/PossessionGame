#include "InteractableObject.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/UI/SecondaryWidgets/InteractableUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

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
	DelegateManager->OnInteractionTry.AddUObject(this, &AInteractableObject::Interact);
	SetupInteractionLogic();
	InteractableUI = Cast<UInteractableUI>(InteractableWidget->GetWidget());
}

void AInteractableObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DisableOutlineHandle);
	GetWorldTimerManager().ClearTimer(FadeOutAnimHandle);
	Super::EndPlay(EndPlayReason);
}

void AInteractableObject::ChangeOutlineState(const bool bState)
{
	if (bIsSkeletalMesh) GetSkeletalMesh()->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	else GetMesh()->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) GetWorldTimerManager().SetTimer(DisableOutlineHandle, this, &AInteractableObject::DisableOutline, 0.245f, false);
}

void AInteractableObject::FadeOutAnimFinished() const { if (InteractableWidget)  InteractableWidget->SetVisibility(false); }

void AInteractableObject::SetupInteractionLogic()
{
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AInteractableObject::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AInteractableObject::DisableInteractionWidget);
}

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
