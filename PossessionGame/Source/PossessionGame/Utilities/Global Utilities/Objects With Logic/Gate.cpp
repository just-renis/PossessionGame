#include "Gate.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void AGate::OpenGate()
{
	UKismetSystemLibrary::K2_SetTimer(this, "MoveGateSides", 0.025f, true);
}

AGate::AGate()
{
	FirstGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("firstGateSide"));
	SecondGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("secondGateSide"));
	RootComponent = FirstGateSide;
	SecondGateSide->SetupAttachment(RootComponent);
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
	if (bIsTutorialGate) GetGameInstance()->GetSubsystem<UDelegateManager>()->OpenTutorialGate.BindUObject(this, &AGate::OpenGate);
}

void AGate::MoveGateSides()
{
	FirstGateSide->SetRelativeLocation(FirstGateSide->GetRelativeLocation() - FVector(3.f, 0.f, 0.f));
	SecondGateSide->SetRelativeLocation(SecondGateSide->GetRelativeLocation() - FVector(0.f, 6.f, 0.f));
	TimePassed += 0.025f;
	if (TimePassed >= 3.f) UKismetSystemLibrary::K2_ClearTimer(this, "MoveGateSides");
}
