#include "TutorialMonitorComponent.h"

void UTutorialMonitorComponent::Init(const TObjectPtr<AEnemy>& InPlayer)
{
	Player = InPlayer;
	UE_LOG(LogTemp, Error, TEXT("NIGGA"));
}
