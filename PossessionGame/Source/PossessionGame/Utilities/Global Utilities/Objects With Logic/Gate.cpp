#include "Gate.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

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
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
	if (InteractableWidget) InteractableWidget->SetVisibility(false);
	if (bIsTutorialGate) GetGameInstance()->GetSubsystem<UDelegateManager>()->OpenTutorialGate.BindUObject(this, &AGate::OpenGate);
}

void AGate::OpenGate()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.AddUObject(this, &AGate::CloseGate);
	UKismetSystemLibrary::K2_SetTimer(this, "OpenGateSides", 0.025f, true);
}

void AGate::CloseGate(const EGameMode InGameMode)
{
	bEventToCloseGateHappened = true;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.RemoveAll(this);
	if (bIsGateOpened)
	{
		Collision->SetCollisionResponseToAllChannels(ECR_Block);
		UKismetSystemLibrary::K2_SetTimer(this, "CloseGateSides", 0.025f, true);
	}
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
