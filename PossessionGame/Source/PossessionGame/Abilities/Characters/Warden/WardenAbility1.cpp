#include "WardenAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Warden.h"
#include "PossessionGame/AI/Enemies/Controllers/WardenAIController.h"

void UWardenAbility1::AI_Execute()
{
	UE_LOG(LogTemp, Error, TEXT("AI_Execute"));
	Proprietor->SetIsActionInMotion(true);
	Proprietor->TriggerBeam();
	Proprietor->GetWorldTimerManager().SetTimer(BeamHandle, this, &UWardenAbility1::TriggerBeamEnd, TriggerBeamEndTime);
	//Proprietor->PlaySound(ESound::RangerGrenadeLaunchAim, true);
	Super::AI_Execute();
}

void UWardenAbility1::AI_StopAbility()
{
	Proprietor->GetWorldTimerManager().ClearTimer(BeamHandle);
	Super::AI_StopAbility();
}

bool UWardenAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWardenAbility1::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AWarden>(AbilityProprietor); }

void UWardenAbility1::TriggerBeamEnd() const
{
	Proprietor->SetIsActionInMotion(false);
	(void)Cast<AWardenAIController>(Proprietor->GetAI())->OnBeamShot.ExecuteIfBound();
}
