#include "GameModeInitiator.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

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
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor))
	{
		if (Enemy->GetIsPossessed())
		{
			if (GameMode == EGameMode::ChargeTerminalMode) GetGameInstance()->GetSubsystem<UDelegateManager>()->SetIsUsbDroppable(true);
			GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.Broadcast(GameMode);
			Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Destroy();
		}
	}
}
