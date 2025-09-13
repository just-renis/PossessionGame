#include "TricksterAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTricksterAbility3::P_Execute()
{
	if (!GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetIsEnemiesAvailable())
	{
		DelegateManager->RequestMinorAnnouncement.ExecuteIfBound(NSLOCTEXT("ST_Localization", "NoTargetsAvailable", "No Targets Available"));
		return;
	}
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::DeployClone);
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const int32 CloneAmount = RunSaveSlot->GetUpgradeAttributeValues(Data->GetAbilityName(), EUpgradeAttribute::CloneAmount, true) + 1;
	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnClone(ECharacterType::Minion, Proprietor, CloneAmount);
	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::CloneSpawn);
	Super::P_Execute();
}

void UTricksterAbility3::P_StopAbility()
{
	if (!bIsAbilityInDuration) return;
	DelegateManager->DespawnPlayerClones.Broadcast();
	Super::P_StopAbility();
}

bool UTricksterAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTricksterAbility3::SetProprietor(TObjectPtr<AEnemy> InAbilityProprietor) { Proprietor = Cast<ATrickster>(InAbilityProprietor); }
