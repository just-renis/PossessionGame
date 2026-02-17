#include "SpyAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"

void USpyAbility1::AI_Execute()
{
	if (CanExecute())
	{
		bIsAbilityInDuration = true;
		Super::AI_Execute();
	}
}

void USpyAbility1::AI_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		Super::AI_StopAbility();
	}
}

void USpyAbility1::P_Execute()
{
	bIsAbilityInDuration = true;
	Super::P_Execute();
}

void USpyAbility1::P_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		Super::P_StopAbility();
	}
}

bool USpyAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USpyAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASpy>(InAbilityProprietor); }
