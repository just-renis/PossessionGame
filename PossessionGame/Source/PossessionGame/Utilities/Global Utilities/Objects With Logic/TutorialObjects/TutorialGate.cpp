#include "TutorialGate.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/UI/SecondaryWidgets/InteractableUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"

ATutorialGate::ATutorialGate()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CloseTriggerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseTriggerCollision"));
	FirstGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstGateSide"));
	SecondGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondGateSide"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableWidgetComponent"));
	RootComponent = Collision;
	CloseTriggerCollision->SetupAttachment(RootComponent);
	CloseTriggerCollision->SetBoxExtent(FVector(10.f, 450.f, 175.f));
	CloseTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CloseTriggerCollision->SetRelativeLocation(FVector(-150.f, 0.f, 0.f));
	CloseTriggerCollision->SetCollisionProfileName("TriggerPlayer");
	CloseTriggerCollision->CanCharacterStepUpOn = ECB_No;
	FirstGateSide->SetupAttachment(RootComponent);
	SecondGateSide->SetupAttachment(RootComponent);
	InteractableWidget->SetupAttachment(RootComponent);
	InteractableWidget->SetVisibility(false);
}

void ATutorialGate::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnInteractionTry.AddUObject(this, &ATutorialGate::Interact);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeFinished.AddUObject(this, &ATutorialGate::OpenGateAndShowMarker);
	InteractableUI = Cast<UInteractableUI>(InteractableWidget->GetWidget());
	if (bIsUnlockableByInteraction) SetupInteractability();
	if (bIsGateClosedByTrigger)
	{
		CloseTriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ATutorialGate::CloseTriggerPassed);
		CloseTriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	GetWorld()->GetSubsystem<UTutorialManager>()->OnMinionDeathCompleted.AddUObject(this, &ATutorialGate::OpenGateOnTutorialMinionDeath);
}

void ATutorialGate::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(OpenGatesHandle);
	GetWorldTimerManager().ClearTimer(CloseGatesHandle);
	GetWorldTimerManager().ClearTimer(DisableOutlineHandle);
	GetWorldTimerManager().ClearTimer(FadeOutAnimHandle);
	Super::EndPlay(EndPlayReason);
}

void ATutorialGate::OpenGate(const EGameMode InGameMode)
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySoundAtLocation(ESound::GatesOpen, GetActorLocation());
	if (GameModeToFinish == InGameMode) GetWorldTimerManager().SetTimer(OpenGatesHandle, this, &ATutorialGate::OpenGateSides, 0.025f, true);
}

void ATutorialGate::OpenGateAndShowMarker(const EGameMode InGameMode)
{
	OpenGate(InGameMode);
	ShowMarker(InGameMode);
}

void ATutorialGate::OpenGateOnTutorialMinionDeath()
{
	OpenGate(EGameMode::TutorialMode1);
	GetWorld()->GetSubsystem<UTutorialManager>()->OnMinionDeathCompleted.RemoveAll(this);
}

void ATutorialGate::CloseGate(const EGameMode InGameMode)
{
	bEventToCloseGateHappened = true;
	if (bIsGateOpened)
	{
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeFinished.RemoveAll(this);
		if (bIsUnlockableByInteraction) Collision->SetBoxExtent(FVector(1.f, 450.f, 175.f));
		Collision->SetCollisionResponseToAllChannels(ECR_Block);
		GetWorldTimerManager().SetTimer(CloseGatesHandle, this, &ATutorialGate::CloseGateSides, 0.025f, true);
	}
}

void ATutorialGate::Interact()
{
	if (!GetIsInteractable()) return;
	SetIsInteractable(false);
	Collision->OnComponentBeginOverlap.RemoveAll(this);
	Collision->OnComponentEndOverlap.RemoveAll(this);
	if (InteractableUI) InteractableUI->PlayFadeInAnim(false);
	GetWorldTimerManager().SetTimer(FadeOutAnimHandle, this, &ATutorialGate::FadeOutAnimFinished, InteractableUI->GetFadeInAnimLength(), false);
	OpenGate(EGameMode::None);
}

void ATutorialGate::ChangeOutlineState(const bool bState)
{
	FirstGateSide->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	SecondGateSide->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) GetWorldTimerManager().SetTimer(DisableOutlineHandle, this, &ATutorialGate::DisableOutline, 0.245f, false);
}

void ATutorialGate::ShowMarker(const EGameMode InGameMode) const
{
	if (GameModeToFinish != InGameMode) return;
	GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * 100.f);
}

void ATutorialGate::FadeOutAnimFinished() const { if (InteractableWidget) InteractableWidget->SetVisibility(false); }

void ATutorialGate::OpenGateSides()
{
	FirstGateSide->SetRelativeLocation(FirstGateSide->GetRelativeLocation() + FVector(0.f, 9.f, 0.f));
	SecondGateSide->SetRelativeLocation(SecondGateSide->GetRelativeLocation() - FVector(0.f, 9.f, 0.f));
	TimePassed += 0.025f;
	if (TimePassed >= 1.f)
	{
		bIsGateOpened = true;
		TimePassed = 0.f;
		GetWorldTimerManager().ClearTimer(OpenGatesHandle);
		if (bEventToCloseGateHappened) CloseGate();
	}
}

void ATutorialGate::CloseGateSides()
{
	FirstGateSide->SetRelativeLocation(FirstGateSide->GetRelativeLocation() - FVector(0.f, 9.f, 0.f));
	SecondGateSide->SetRelativeLocation(SecondGateSide->GetRelativeLocation() + FVector(0.f, 9.f, 0.f));
	TimePassed += 0.025f;
	if (TimePassed >= 1.f)
	{
		if (bIsTutorialBotChangeLocationGate) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGateClosed.ExecuteIfBound();
		GetWorldTimerManager().ClearTimer(CloseGatesHandle);
	}
}

void ATutorialGate::SetupInteractability()
{
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATutorialGate::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATutorialGate::DisableInteractionWidget);
	Collision->SetBoxExtent(FVector(250.f, 450.f, 175.f));
}

void ATutorialGate::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	if (InteractableWidget) InteractableWidget->SetVisibility(true);
	if (InteractableUI) InteractableUI->PlayFadeInAnim(true);
	SetIsInteractable(true);
}

void ATutorialGate::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	if (InteractableUI) InteractableUI->PlayFadeInAnim(false);
	GetWorldTimerManager().SetTimer(FadeOutAnimHandle, this, &ATutorialGate::FadeOutAnimFinished, InteractableUI->GetFadeInAnimLength(), false);
	SetIsInteractable(false);
}

void ATutorialGate::CloseTriggerPassed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	CloseGate();
	CloseTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
