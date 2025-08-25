#include "SoundTrackTrigger.h"
#include "Components/BoxComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ASoundTrackTrigger::ASoundTrackTrigger()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->SetCollisionProfileName(FName("TriggerPlayer"));
}

void ASoundTrackTrigger::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASoundTrackTrigger::Initiate);
}

void ASoundTrackTrigger::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnMusicSoundtrackChangeRequired.ExecuteIfBound(SoundTrackName);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
