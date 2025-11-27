#include "BreacherAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Breacher.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UBreacherAbility3::P_Execute()
{
	if (!GetWorld()->GetSubsystem<UPoolingManager>()->CanObjectBeThrown(EPoolObject::BreacherDynamite))
	{
		DelegateManager->RequestMinorAnnouncement.ExecuteIfBound(NSLOCTEXT("ST_Localization", "DynamiteDetonationInProgress", "Dynamite Detonation in Progress"), ESound::NotSelectable);
		return;
	}
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::BreacherThrow);
	Proprietor->P_ThrowDynamite();
	Super::P_Execute();
}

bool UBreacherAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UBreacherAbility3::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ABreacher>(AbilityProprietor); }
