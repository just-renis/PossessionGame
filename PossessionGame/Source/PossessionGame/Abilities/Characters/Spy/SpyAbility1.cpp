#include "SpyAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Spy.h"

void USpyAbility1::AI_Execute()
{
	if (CanExecute())
	{
		UE_LOG(LogTemp, Warning, TEXT("I USED ABILITY"));
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
		UE_LOG(LogTemp, Warning, TEXT("ABILITY ENDED"));
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

bool USpyAbility1::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void USpyAbility1::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ASpy>(AbilityProprietor); }
