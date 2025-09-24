#include "STTaskWardenBeamShot.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/WardenAIController.h"

EStateTreeRunStatus USTTaskWardenBeamShot::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Error, TEXT("USTTaskWardenBeamShot::EnterState"));
	CachedController = Cast<AWardenAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::WardenBeamShot);
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnBeamShot.BindUObject(this, &USTTaskWardenBeamShot::BeamShot);
	return Super::EnterState(Context, Transition);
}

void USTTaskWardenBeamShot::BeamShot()
{
	UE_LOG(LogTemp, Error, TEXT("BeamShot"));
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
