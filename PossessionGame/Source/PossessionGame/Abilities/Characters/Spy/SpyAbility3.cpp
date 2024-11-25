#include "SpyAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void USpyAbility3::P_Execute()
{
	DelegateManager->OnXRayThroughWallsChange.Broadcast(true);
	Proprietor->SetXRayState(true);
	bIsAbilityInDuration = true;
	Super::P_Execute();
}

void USpyAbility3::P_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		Super::P_StopAbility();
		Proprietor->SetXRayState(false);
		DelegateManager->OnXRayThroughWallsChange.Broadcast(false);
	}
}

bool USpyAbility3::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void USpyAbility3::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ASpy>(AbilityProprietor); }
