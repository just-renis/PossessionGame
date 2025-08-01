#include "GameModeInitiator.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

AGameModeInitiator::AGameModeInitiator()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RootComponent = Collision;
}

void AGameModeInitiator::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AGameModeInitiator::Initiate);
}

void AGameModeInitiator::Initiate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		if (GameMode == EGameMode::ChargeTerminalMode) GetWorld()->GetSubsystem<UEnemyCommunicator>()->SetIsUsbDroppable(true);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.Broadcast(GameMode);
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnMusicSoundtrackChangeRequired.ExecuteIfBound();
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Destroy();
	}
}
