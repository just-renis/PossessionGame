#include "ExplanationTrigger.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"

AExplanationTrigger::AExplanationTrigger()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
}

void AExplanationTrigger::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AExplanationTrigger::Initiate);
}

void AExplanationTrigger::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	GetWorld()->GetSubsystem<UTutorialManager>()->ShowExplanation.Broadcast(Explanation);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
