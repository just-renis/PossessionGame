#include "SamuraiAIController.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"

void ASamuraiAIController::BeginPlay()
{
	Super::BeginPlay();
	bIsControlRigAvailableToToggle = false;
	SetIsUsingEQS(false);
	EQSTarget = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetTarget();
}
