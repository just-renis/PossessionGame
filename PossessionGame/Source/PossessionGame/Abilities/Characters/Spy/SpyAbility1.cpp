#include "SpyAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"

void USpyAbility1::P_Execute()
{
	Super::P_Execute();
	Proprietor->SetFirstAbilityActiveState(true);
}

void USpyAbility1::P_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		Proprietor->SetFirstAbilityActiveState(false);
		Super::P_StopAbility();
	}
}

bool USpyAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USpyAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASpy>(InAbilityProprietor); }
