#include "SpyAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"

void USpyAbility1::AI_Execute()
{
	if (CanExecute())
	{
		bIsAbilityInDuration = true;
		Proprietor->SetConceal(true);
		Super::AI_Execute();
	}
}

void USpyAbility1::AI_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		Super::AI_StopAbility();
		Proprietor->SetConceal(false);
	}
}

void USpyAbility1::P_Execute()
{
//		DelegateManager->OnInvisibilityChange.Broadcast(false); // CLONE
	bIsAbilityInDuration = true;
	Proprietor->SetConceal(true);
	Super::P_Execute();
}

void USpyAbility1::P_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		Super::P_StopAbility();
		Proprietor->SetConceal(false);
	//	DelegateManager->OnInvisibilityChange.Broadcast(true); // CLONE DESTROYED SEND BACK PLAYER POSITION
	}
}

bool USpyAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USpyAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASpy>(InAbilityProprietor); }
