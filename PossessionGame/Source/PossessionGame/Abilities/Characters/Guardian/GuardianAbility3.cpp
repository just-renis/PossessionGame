#include "GuardianAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Guardian.h"

void UGuardianAbility3::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->P_LaunchVectorBlast();
	UE_LOG(LogTemp, Error, TEXT("EXECUTE"));
	Super::P_Execute();
}

bool UGuardianAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UGuardianAbility3::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AGuardian>(AbilityProprietor); }
