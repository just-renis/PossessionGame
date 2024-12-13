#include "RangerAIController.h"

void ARangerAIController::AdjustCustomFocus() const
{
	if (!Proprietor->GetIsActionInMotion()) Super::AdjustCustomFocus();
}
