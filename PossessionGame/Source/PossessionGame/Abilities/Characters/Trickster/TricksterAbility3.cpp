#include "TricksterAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UTricksterAbility3::P_Execute()
{
	Super::P_Execute();
	UE_LOG(LogTemp, Error, TEXT("P_EXECUTE"));
	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnClone(ECharacterType::Minion, Proprietor);
}

bool UTricksterAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTricksterAbility3::SetProprietor(TObjectPtr<AEnemy> InAbilityProprietor) { Proprietor = Cast<ATrickster>(InAbilityProprietor); }
