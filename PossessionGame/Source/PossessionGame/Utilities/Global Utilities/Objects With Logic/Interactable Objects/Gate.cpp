#include "Gate.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/UI/MainWidgets/DemoEndUI.h"
#include "PossessionGame/UI/SecondaryWidgets/InteractableUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

AGate::AGate()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("collision"));
	CloseTriggerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseTriggerCollision"));
	OpenTriggerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OpenTriggerCollision"));
	FirstGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("firstGateSide"));
	SecondGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("secondGateSide"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	RootComponent = Collision;
	CloseTriggerCollision->SetupAttachment(RootComponent);
	CloseTriggerCollision->SetBoxExtent(FVector(10.f, 450.f, 175.f));
	CloseTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CloseTriggerCollision->SetRelativeLocation(FVector(-150.f, 0.f, 0.f));
	CloseTriggerCollision->SetCollisionProfileName("TriggerPlayer");
	CloseTriggerCollision->CanCharacterStepUpOn = ECB_No;
	OpenTriggerCollision->SetupAttachment(RootComponent);
	OpenTriggerCollision->SetBoxExtent(FVector(20.f, 450.f, 450.f));
	OpenTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OpenTriggerCollision->SetRelativeLocation(FVector(150.f, 0.f, 0.f));
	OpenTriggerCollision->SetCollisionProfileName("TriggerPlayer");
	OpenTriggerCollision->CanCharacterStepUpOn = ECB_No;
	FirstGateSide->SetupAttachment(RootComponent);
	SecondGateSide->SetupAttachment(RootComponent);
	InteractableWidget->SetupAttachment(RootComponent);
	InteractableWidget->SetVisibility(false);
}

bool AGate::GetIsInteractable() const { return IInteractable::GetIsInteractable() && !bNeedsArtifact; }

void AGate::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnInteractionTry.AddUObject(this, &AGate::Interact);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeFinished.AddUObject(this, &AGate::OpenGateAndShowMarker);
	if (bIsGateAssociatedWithOtherGates) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGateWithAssociationOpened.AddUObject(this, &AGate::DisableGate);
	InteractableUI = Cast<UInteractableUI>(InteractableWidget->GetWidget());
	if (bIsUnlockableByInteraction) SetupInteractability();
	if (bIsGateClosedByTrigger)
	{
		CloseTriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &AGate::CloseTriggerPassed);
		CloseTriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	if (bIsGateOpenedByTrigger)
	{
		OpenTriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &AGate::OpenTriggerPassed);
		OpenTriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	if (bNeedsArtifact) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnArtifactCollected.AddUObject(this, &AGate::SetArtifactCollected);
}

void AGate::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(OpenGatesHandle);
	GetWorldTimerManager().ClearTimer(CloseGatesHandle);
	GetWorldTimerManager().ClearTimer(DisableOutlineHandle);
	GetWorldTimerManager().ClearTimer(FadeOutAnimHandle);
	Super::EndPlay(EndPlayReason);
}

void AGate::OpenGate(const EGameMode InGameMode)
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySoundAtLocation(ESound::GatesOpen, GetActorLocation());
	if (GameModeToFinish == InGameMode) GetWorldTimerManager().SetTimer(OpenGatesHandle, this, &AGate::OpenGateSides, 0.025f, true);
}

void AGate::OpenGateAndShowMarker(const EGameMode InGameMode)
{
	OpenGate(InGameMode);
	ShowMarker(InGameMode);
}

void AGate::CloseGate()
{
	bEventToCloseGateHappened = true;
	Collision->SetBoxExtent(FVector(1.f, 450.f, 175.f));
	Collision->SetCollisionResponseToAllChannels(ECR_Block);
	if (bIsGateOpened)
	{
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeFinished.RemoveAll(this);
		GetWorldTimerManager().SetTimer(CloseGatesHandle, this, &AGate::CloseGateSides, 0.025f, true);
	}
}

void AGate::Interact()
{
	if (!GetIsInteractable() || bNeedsArtifact) return;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnInteractionTry.RemoveAll(this);
	if (bIsGateAssociatedWithOtherGates)
	{
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGateWithAssociationOpened.RemoveAll(this);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGateWithAssociationOpened.Broadcast(GateAssociationId);
	}
	SetIsInteractable(false);
	Collision->OnComponentBeginOverlap.RemoveAll(this);
	Collision->OnComponentEndOverlap.RemoveAll(this);
	if (InteractableUI) InteractableUI->PlayFadeInAnim(false);
	GetWorldTimerManager().SetTimer(FadeOutAnimHandle, this, &AGate::FadeOutAnimFinished, InteractableUI->GetFadeInAnimLength(), false);
	OpenGate(EGameMode::None);
	if (bIsDemoGate) GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UDemoEndUI>(EWidget::DemoEndUI);
}

void AGate::ChangeOutlineState(const bool bState)
{
	FirstGateSide->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	SecondGateSide->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) GetWorldTimerManager().SetTimer(DisableOutlineHandle, this, &AGate::DisableOutline, 0.245f, false);
}

void AGate::SetArtifactCollected()
{
	bNeedsArtifact = false;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnArtifactCollected.RemoveAll(this);
}

void AGate::ShowMarker(const EGameMode InGameMode) const
{
	if (GameModeToFinish != InGameMode) return;
	GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation() + FVector::UpVector * 100.f);
}

void AGate::FadeOutAnimFinished() const { if (InteractableWidget) InteractableWidget->SetVisibility(false); }

void AGate::OpenGateSides()
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

void AGate::CloseGateSides()
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

void AGate::SetupInteractability()
{
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AGate::EnableInteractionWidget);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AGate::DisableInteractionWidget);
	Collision->SetBoxExtent(FVector(150.f, 450.f, 175.f));
}

void AGate::DisableGate(const int32 InAssociationIndex)
{
	if (GateAssociationId == InAssociationIndex)
	{
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGateWithAssociationOpened.RemoveAll(this);
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Collision->OnComponentBeginOverlap.RemoveAll(this);
		Collision->OnComponentEndOverlap.RemoveAll(this);
		bIsTutorialBotChangeLocationGate = false;
		bIsUnlockableByInteraction = false;
		bIsGateClosedByTrigger = false;
	}
}

void AGate::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bNeedsArtifact || !OtherActor->ActorHasTag("Player")) return;
	if (InteractableWidget) InteractableWidget->SetVisibility(true);
	if (InteractableUI) InteractableUI->PlayFadeInAnim(true);
	SetIsInteractable(true);
}

void AGate::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bNeedsArtifact || !OtherActor->ActorHasTag("Player")) return;
	if (InteractableUI) InteractableUI->PlayFadeInAnim(false);
	GetWorldTimerManager().SetTimer(FadeOutAnimHandle, this, &AGate::FadeOutAnimFinished, InteractableUI->GetFadeInAnimLength(), false);
	SetIsInteractable(false);
}

void AGate::CloseTriggerPassed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	CloseGate();
	CloseTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGate::OpenTriggerPassed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player") || bNeedsArtifact) return;
	OpenGate(EGameMode::None);
	OpenTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (bIsDemoGate) GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UDemoEndUI>(EWidget::DemoEndUI);
}
