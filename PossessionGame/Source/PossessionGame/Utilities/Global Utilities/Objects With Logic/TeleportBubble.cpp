#include "TeleportBubble.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

void ATeleportBubble::Teleport(const TObjectPtr<AActor>& InActor) const { InActor->SetActorLocation(GetActorLocation()); }

void ATeleportBubble::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->AddTeleportBubble(this);
}
