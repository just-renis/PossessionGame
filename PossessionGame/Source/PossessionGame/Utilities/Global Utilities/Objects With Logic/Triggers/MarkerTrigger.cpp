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
	GetWorld()->GetSubsystem<UMarkerManager>()->GetOnMarkerTriggeredDelegate().AddUObject(this, &AMarkerTrigger::OnOtherMarkerTriggerPassed);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AMarkerTrigger::Initiate);
}

void AMarkerTrigger::OnOtherMarkerTriggerPassed(const int32 InNextMarkerToTrigger) const
{
	if (InNextMarkerToTrigger == MarkerTriggerIndex) GetWorld()->GetSubsystem<UMarkerManager>()->SpawnMarkerAtLocation(GetActorLocation());
}

void AMarkerTrigger::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	GetWorld()->GetSubsystem<UMarkerManager>()->DestroyMarkers();
	GetWorld()->GetSubsystem<UMarkerManager>()->GetOnMarkerTriggeredDelegate().RemoveAll(this);
	if (NextMarkerToTrigger != -1) GetWorld()->GetSubsystem<UMarkerManager>()->GetOnMarkerTriggeredDelegate().Broadcast(NextMarkerToTrigger);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
