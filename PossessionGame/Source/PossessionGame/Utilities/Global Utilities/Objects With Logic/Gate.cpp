#include "Gate.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

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
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	if (bIsUnlockableByInteraction)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AGate::EnableInteractionWidget);
		Collision->OnComponentEndOverlap.AddDynamic(this, &AGate::DisableInteractionWidget);
		Collision->SetBoxExtent(FVector(250.f, 450.f, 175.f));
		SetIsInteractable(true);
	}
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeFinished.AddUObject(this, &AGate::OpenGate);
}

void AGate::OpenGate(const EGameMode InGameMode)
{
	if (GameModeToFinish == InGameMode)
	{
		OpenGatesSfx->Sound ? OpenGatesSfx->Play() : GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(OpenGatesSfx, ESound::GatesOpen);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.AddUObject(this, &AGate::CloseGate);
		UKismetSystemLibrary::K2_SetTimer(this, "OpenGateSides", 0.025f, true);
	}
}

void AGate::CloseGate(const EGameMode InGameMode)
{
	bEventToCloseGateHappened = true;
	if (bIsGateOpened)
	{
		if (bIsUnlockableByInteraction) Collision->SetBoxExtent(FVector(1.f, 450.f, 175.f));
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.RemoveAll(this);
		Collision->SetCollisionResponseToAllChannels(ECR_Block);
		UKismetSystemLibrary::K2_SetTimer(this, "CloseGateSides", 0.025f, true);
	}
}

void AGate::Interact()
{
	if (!GetIsInteractable()) return;
	SetIsInteractable(false);
	Collision->OnComponentBeginOverlap.RemoveAll(this);
	Collision->OnComponentEndOverlap.RemoveAll(this);
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	OpenGate(EGameMode::None);
}

void AGate::ChangeOutlineState(const bool bState)
{
	FirstGateSide->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	SecondGateSide->SetOverlayMaterial(bState ? MI_InteractionOutline : nullptr);
	if (bState) UKismetSystemLibrary::K2_SetTimer(this, "DisableOutline", 0.245f, false);
}

void AGate::OpenGateSides()
{
	FirstGateSide->SetRelativeLocation(FirstGateSide->GetRelativeLocation() + FVector(0.f, 6.f, 0.f));
	SecondGateSide->SetRelativeLocation(SecondGateSide->GetRelativeLocation() - FVector(0.f, 6.f, 0.f));
	TimePassed += 0.025f;
	if (TimePassed >= 1.5f)
	{
		bIsGateOpened = true;
		TimePassed = 0.f;
		UKismetSystemLibrary::K2_ClearTimer(this, "OpenGateSides");
		if (bEventToCloseGateHappened) CloseGate();
	}
}

void AGate::CloseGateSides()
{
	FirstGateSide->SetRelativeLocation(FirstGateSide->GetRelativeLocation() - FVector(0.f, 6.f, 0.f));
	SecondGateSide->SetRelativeLocation(SecondGateSide->GetRelativeLocation() + FVector(0.f, 6.f, 0.f));
	TimePassed += 0.025f;
	if (TimePassed >= 1.5f) UKismetSystemLibrary::K2_ClearTimer(this, "CloseGateSides");
}

void AGate::EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(true);
	}
}

void AGate::DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed()) InteractableWidget->SetVisibility(false);
	}
}
