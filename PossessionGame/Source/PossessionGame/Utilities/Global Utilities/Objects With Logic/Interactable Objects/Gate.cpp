#include "Gate.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/UI/MainWidgets/DemoEndUI.h"
#include "PossessionGame/UI/SecondaryWidgets/InteractableUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/Marker.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

AGate::AGate()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("collision"));
	FirstGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("firstGateSide"));
	SecondGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("secondGateSide"));
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("interactableWidgetComponent"));
	RootComponent = Collision;
	FirstGateSide->SetupAttachment(RootComponent);
	SecondGateSide->SetupAttachment(RootComponent);
	InteractableWidget->SetupAttachment(RootComponent);
	OpenGatesSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("openGatesSfx"));
	OpenGatesSfx->SetAutoActivate(false);
	OpenGatesSfx->SetupAttachment(RootComponent);
	InteractableWidget->SetVisibility(false);
}

bool AGate::GetIsInteractable() const { return IInteractable::GetIsInteractable() && !bNeedsArtifact; }

void AGate::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnInteractionTry.AddUObject(this, &AGate::Interact);
	InteractableUI = Cast<UInteractableUI>(InteractableWidget->GetWidget());
	bNeededArtifact = bNeedsArtifact;
	if (bIsUnlockableByInteraction) SetupInteractability();
	if (!bIsTutorialGate) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeFinished.AddUObject(this, &AGate::OpenGateAndShowMarker);
	else if (!Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished())
	{
		GetWorld()->GetSubsystem<UTutorialManager>()->OnMinionDeathCompleted.AddUObject(this, &AGate::OpenAsTutorialGate);
	}
	if (bNeedsArtifact) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnArtifactCollected.BindUObject(this, &AGate::SetArtifactCollected);
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
	if (GameModeToFinish == InGameMode)
	{
		OpenGatesSfx->Sound ? OpenGatesSfx->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(OpenGatesSfx, nullptr, ESound::GatesOpen);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.AddUObject(this, &AGate::CloseGate);
		GetWorldTimerManager().SetTimer(OpenGatesHandle, this, &AGate::OpenGateSides, 0.025f, true);
	}
}

void AGate::OpenGateAndShowMarker(const EGameMode InGameMode)
{
	OpenGate(InGameMode);
	ShowMarker(InGameMode);
}

void AGate::OpenAsTutorialGate()
{
	OpenGate(EGameMode::TutorialMode1);
	GetWorld()->GetSubsystem<UTutorialManager>()->OnMinionDeathCompleted.RemoveAll(this);
}

void AGate::CloseGate(const EGameMode InGameMode)
{
	bEventToCloseGateHappened = true;
	if (bIsGateOpened)
	{
		if (bIsUnlockableByInteraction) Collision->SetBoxExtent(FVector(1.f, 450.f, 175.f));
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.RemoveAll(this);
		Collision->SetCollisionResponseToAllChannels(ECR_Block);
		GetWorldTimerManager().SetTimer(CloseGatesHandle, this, &AGate::CloseGateSides, 0.025f, true);
	}
}

void AGate::Interact()
{
	if (!GetIsInteractable() || bNeedsArtifact) return;
	if (bNeededArtifact) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretDoorOpened.ExecuteIfBound();
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
	if (bState) GetWorldTimerManager().SetTimer(DisableOutlineHandle, this, &AGate::DisableOutline, 0.0245f, false);
}

void AGate::ShowMarker(const EGameMode InGameMode)
{
	if (GameModeToFinish != InGameMode) return;
	if (!Marker) Marker = GetWorld()->SpawnActor<AMarker>(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetMarkerClass(), GetActorLocation() + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
	Marker->Toggle(true);
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
	if (Marker) Marker->DestroyThis();
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
	Collision->SetBoxExtent(FVector(250.f, 450.f, 175.f));
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
