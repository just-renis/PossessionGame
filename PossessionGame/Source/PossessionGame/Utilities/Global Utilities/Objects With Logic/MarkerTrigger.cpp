#include "MarkerTrigger.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Utilities/Subsystems/MarkerManager.h"

AMarkerTrigger::AMarkerTrigger()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
}

void AMarkerTrigger::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AMarkerTrigger::Initiate);
}

void AMarkerTrigger::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	if (NextMarkerToTrigger != -1) (void)GetWorld()->GetSubsystem<UMarkerManager>()->GetOnMarkerTriggeredDelegate().ExecuteIfBound(NextMarkerToTrigger);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
