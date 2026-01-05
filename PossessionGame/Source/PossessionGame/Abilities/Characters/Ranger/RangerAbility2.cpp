#include "RangerAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAbility2::AI_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::DeployShield);
	Proprietor->SpawnShield();
	Super::AI_Execute();
}

void URangerAbility2::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::DeployShield);
	Proprietor->SpawnShield();
	if (bRangerBlueVariationEnabled) GetWorld()->GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddCriticalDamageStack();
	Super::P_Execute();
}

bool URangerAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void URangerAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ARanger>(InAbilityProprietor); }

void URangerAbility2::Setup(TObjectPtr<AEnemy> InAbilityOwner, TObjectPtr<UAbilityData> InAbilityData, TObjectPtr<UDelegateManager> InDelegateManager)
{
	Super::Setup(InAbilityOwner, InAbilityData, InDelegateManager);
	bRangerBlueVariationEnabled = GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ECharacterType::Ranger) == EVariation::RangerVariationBlue;
}
