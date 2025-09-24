#include "WardenAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Warden.h"
#include "PossessionGame/AI/Enemies/Controllers/WardenAIController.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UWardenAbility1::AI_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->TriggerBeam();
	Proprietor->GetWorldTimerManager().SetTimer(BeamShakeHandle, this, &UWardenAbility1::TriggerBeamShake, TriggerBeamShakeTime);
	Proprietor->GetWorldTimerManager().SetTimer(BeamHandle, this, &UWardenAbility1::TriggerBeamEnd, TriggerBeamEndTime);
	//Proprietor->PlaySound(ESound::RangerGrenadeLaunchAim, true);
	Super::AI_Execute();
}

void UWardenAbility1::AI_StopAbility()
{
	Proprietor->GetWorldTimerManager().ClearTimer(BeamShakeHandle);
	Proprietor->GetWorldTimerManager().ClearTimer(BeamHandle);
	Super::AI_StopAbility();
}

bool UWardenAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWardenAbility1::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AWarden>(AbilityProprietor); }

void UWardenAbility1::TriggerBeamShake() const { (void)DelegateManager->RequestCameraShake.ExecuteIfBound(Proprietor->GetBeamShotCameraShake()); }

void UWardenAbility1::TriggerBeamEnd() const
{
	Proprietor->SetIsActionInMotion(false);
	(void)Cast<AWardenAIController>(Proprietor->GetAI())->OnBeamShot.ExecuteIfBound();
}
