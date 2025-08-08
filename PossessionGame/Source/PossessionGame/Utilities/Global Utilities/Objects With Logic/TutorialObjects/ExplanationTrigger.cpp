#include "ExplanationTrigger.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

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
	GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().Broadcast(Explanation);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
