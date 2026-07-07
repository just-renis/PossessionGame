#include "TeleportTrigger.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATeleportTrigger::ATeleportTrigger()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
}

void ATeleportTrigger::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTeleporterEntered.AddUObject(this, &ATeleportTrigger::TeleportPlayer);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATeleportTrigger::Initiate);
}

void ATeleportTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTeleporterEntered.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ATeleportTrigger::TeleportPlayer(const int32 InId, const TObjectPtr<AActor>& InPlayer)
{
	if (InId != Id) return;
	InPlayer->SetActorLocation(GetActorLocation());
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTeleporterEntered.RemoveAll(this);
	Destroy();
}

void ATeleportTrigger::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTeleporterEntered.RemoveAll(this);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTeleporterEntered.Broadcast(Id, OtherActor);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
