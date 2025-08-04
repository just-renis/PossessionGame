#include "HintTrigger.h"
#include "Components/BoxComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

AHintTrigger::AHintTrigger()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = Collision;
}

void AHintTrigger::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AHintTrigger::Initiate);
}

void AHintTrigger::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnShowHint.ExecuteIfBound();
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
