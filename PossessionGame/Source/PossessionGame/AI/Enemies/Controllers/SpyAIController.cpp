#include "SpyAIController.h"

void ASpyAIController::ResetOnReachedNonEqsPoint()
{
	SetIsUsingEQS(true);
	Proprietor->AI_OnReachedNonEqsPoint();
}
