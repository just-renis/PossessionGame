#include "GuardianAbility2.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Characters/Guardian.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UGuardianAbility2::P_Execute()
{
	bIsAbilityInDuration = true;
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::GuardianSlowMotion, true);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2f);
	Proprietor->SetCustomTimeDilation(5.f);
	Super::P_Execute();
}

void UGuardianAbility2::P_StopAbility()
{
	if (bIsAbilityInDuration)
	{
		GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->RemoveStoredSound(ESound::GuardianSlowMotion);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
		Proprietor->SetCustomTimeDilation(1.f);
		Super::P_StopAbility();
	}
}

bool UGuardianAbility2::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UGuardianAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AGuardian>(AbilityProprietor); }
