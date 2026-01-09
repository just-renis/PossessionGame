#include "RangerAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAbility3::P_Execute()
{
	if (!GetWorld()->GetSubsystem<UPoolingManager>()->CanObjectBeThrown(EPoolObject::RangerMine))
	{
		DelegateManager->RequestMinorAnnouncement.ExecuteIfBound(NSLOCTEXT("ST_Localization", "MineDetonationInProgress", "Mine Detonation in Progress"), ESound::NotSelectable);
		return;
	}
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::Throw);
	Proprietor->P_InitiateThrowMine();
	if (bRangerBlueVariationEnabled) GetWorld()->GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddCriticalDamageStack();
	Super::P_Execute();
}

bool URangerAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void URangerAbility3::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ARanger>(InAbilityProprietor); }

void URangerAbility3::Setup(TObjectPtr<AEnemy> InAbilityOwner, TObjectPtr<UAbilityData> InAbilityData, TObjectPtr<UDelegateManager> InDelegateManager)
{
	Super::Setup(InAbilityOwner, InAbilityData, InDelegateManager);
	bRangerBlueVariationEnabled = GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ECharacterType::Ranger) == EVariation::RangerVariationBlue;
}
