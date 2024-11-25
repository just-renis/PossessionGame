#include "RangerAbility3.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"

void URangerAbility3::P_Execute()
{
	bIsAbilityInDuration = true;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2f);
	Proprietor->SetCustomTimeDilation(5.f);
	Super::P_Execute();
}

void URangerAbility3::P_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
		Proprietor->SetCustomTimeDilation(1.f);
		Super::P_StopAbility();
	}
}

bool URangerAbility3::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void URangerAbility3::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ARanger>(AbilityProprietor); }
