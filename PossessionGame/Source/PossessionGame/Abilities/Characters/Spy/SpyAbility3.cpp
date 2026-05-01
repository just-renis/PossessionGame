#include "SpyAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void USpyAbility3::P_Execute()
{
	Super::P_Execute();
	DelegateManager->OnXrayChange.Broadcast(true);
	Proprietor->SetXrayState(true);
}

void USpyAbility3::P_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		Super::P_StopAbility();
		Proprietor->SetXrayState(false);
		DelegateManager->OnXrayChange.Broadcast(false);
	}
}

bool USpyAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USpyAbility3::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASpy>(InAbilityProprietor); }
