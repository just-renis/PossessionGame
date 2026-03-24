#include "AcoBossAIController.h"
#include "PossessionGame/AI/Enemies/Characters/AcoBoss.h"

FVector AAcoBossAIController::GetFocusTargetLocation() const
{
	if (!RealProprietor) return Super::GetFocusTargetLocation();
	return RealProprietor->GetCurrentTargetLocation();
}

void AAcoBossAIController::BeginPlay()
{
	Super::BeginPlay();
	RealProprietor = Cast<AAcoBoss>(Proprietor);
	SetIsUsingEQS(false);
}
