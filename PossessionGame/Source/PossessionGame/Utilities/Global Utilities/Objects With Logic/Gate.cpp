#include "Gate.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

AGate::AGate()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("collision"));
	FirstGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("firstGateSide"));
	SecondGateSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("secondGateSide"));
	RootComponent = Collision;
	FirstGateSide->SetupAttachment(RootComponent);
	SecondGateSide->SetupAttachment(RootComponent);
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
	if (bIsTutorialGate) GetGameInstance()->GetSubsystem<UDelegateManager>()->OpenTutorialGate.BindUObject(this, &AGate::OpenGate);
}

void AGate::OpenGate()
{
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AGate::GatePassed);
	UKismetSystemLibrary::K2_SetTimer(this, "MoveGateSides", 0.025f, true);
}

void AGate::MoveGateSides()
{
	FirstGateSide->SetRelativeLocation(FirstGateSide->GetRelativeLocation() + FVector(0.f, 3.f, 0.f));
	SecondGateSide->SetRelativeLocation(SecondGateSide->GetRelativeLocation() - FVector(0.f, 3.f, 0.f));
	TimePassed += 0.025f;
	if (TimePassed >= 3.f) UKismetSystemLibrary::K2_ClearTimer(this, "MoveGateSides");
}

void AGate::GatePassed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("WHAT PASSED: %s"), *OtherActor->GetName());
	//(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGatePassed.ExecuteIfBound(NextGameMode);
}
