#include "GameModeInitiator.h"
#include "Components/BoxComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

AGameModeInitiator::AGameModeInitiator()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("collision"));
	Collision->SetCollisionProfileName("TriggerPlayer");
	RootComponent = Collision;
}

void AGameModeInitiator::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AGameModeInitiator::Initiate);
}

void AGameModeInitiator::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	if (bFirstGameMode) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnFirstGameModeInitiated.ExecuteIfBound();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.Broadcast(GameMode);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
