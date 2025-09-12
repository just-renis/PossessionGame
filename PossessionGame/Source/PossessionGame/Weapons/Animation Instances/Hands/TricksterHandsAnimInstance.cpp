#include "TricksterHandsAnimInstance.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Character Weapons/TricksterWeapon.h"

void UTricksterHandsAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* ReloadMontage = Montages.Find(EHandsAnimState::Reload);
	if (ReloadMontage && Montage == ReloadMontage->Get())
	{
		Cast<ATricksterWeapon>(GetProprietor()->GetWeapon())->P_StopOverChargeVfx();
		return;
	}
	const TObjectPtr<UAnimMontage>* SmokeBombThrowMontage = Montages.Find(EHandsAnimState::GrenadeLaunch);
	if (SmokeBombThrowMontage && Montage == SmokeBombThrowMontage->Get())
	{
		(void)OnHandsMontageEnd.ExecuteIfBound(false);
		return;
	}
	const TObjectPtr<UAnimMontage>* EmpThrowMontage = Montages.Find(EHandsAnimState::SecondaryGrenadeLaunch);
	if (EmpThrowMontage && Montage == EmpThrowMontage->Get())
	{
		(void)OnHandsMontageEnd.ExecuteIfBound(false);
		return;
	}
	const TObjectPtr<UAnimMontage>* DeployCloneMontage = Montages.Find(EHandsAnimState::DeployClone);
	if (DeployCloneMontage && Montage == DeployCloneMontage->Get()) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
