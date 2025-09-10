#include "TricksterAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UTricksterAbility3::P_Execute()
{
	UE_LOG(LogTemp, Error, TEXT("P_EXECUTE"));
	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnClone(ECharacterType::Minion, Proprietor, 5);
	Super::P_Execute();
}

void UTricksterAbility3::P_StopAbility()
{
	if (!bIsAbilityInDuration) return;
	UE_LOG(LogTemp, Error, TEXT("P_StopAbility"));
	DelegateManager->DespawnPlayerClones.Broadcast();
	Super::P_StopAbility();
}

bool UTricksterAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTricksterAbility3::SetProprietor(TObjectPtr<AEnemy> InAbilityProprietor) { Proprietor = Cast<ATrickster>(InAbilityProprietor); }
