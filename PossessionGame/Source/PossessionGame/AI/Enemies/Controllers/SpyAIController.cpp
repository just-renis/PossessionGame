#include "SpyAIController.h"

void ASpyAIController::ResetOnReachedNonEqsPoint()
{
	SetIsUsingEQS(true);
	Proprietor->SetIsAllowedToShoot(true);
}
