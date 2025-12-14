#include "TricksterHandsAnimInstance.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Character Weapons/TricksterWeapon.h"

void UTricksterHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EHandsAnimState::Reload, InMontage))
	{
		Cast<ATricksterWeapon>(GetProprietor()->GetWeapon())->P_StopOverChargeVfx();
		return;
	}
	if (IsMontage(EHandsAnimState::GrenadeLaunch, InMontage) || IsMontage(EHandsAnimState::SecondaryGrenadeLaunch, InMontage) || IsMontage(EHandsAnimState::DeployClone, InMontage)) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
