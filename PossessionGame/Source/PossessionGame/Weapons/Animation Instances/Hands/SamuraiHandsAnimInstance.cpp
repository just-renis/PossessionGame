#include "SamuraiHandsAnimInstance.h"
#include "PossessionGame/Enums/EBowState.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Character Weapons/SamuraiWeapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

bool USamuraiHandsAnimInstance::IsCorrectMontage(UAnimMontage* Montage, EHandsAnimState State) { return Montages.Contains(State) && Montage == Montages.Find(State)->Get(); }

void USamuraiHandsAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	if (bIsInterrupted) return;
	if (IsCorrectMontage(Montage, EHandsAnimState::StretchStart))
	{
		Weapon->SetBowState(EBowState::StretchHold);
		PlayMontage(EHandsAnimState::StretchHold);
		Weapon->GetAnimInstance()->PlayMontage(EWeaponAnimState::StretchHold);
	}
	else if (IsCorrectMontage(Montage, EHandsAnimState::Shoot))
	{
		Weapon->SetBowState(EBowState::ShootRelease);
		PlayMontage(EHandsAnimState::StretchRelease);
	}
	else if (Weapon->GetBowState() == EBowState::ShootRelease && IsCorrectMontage(Montage, EHandsAnimState::StretchRelease)) Weapon->SetBowState(EBowState::StretchRelease);
}

void USamuraiHandsAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Weapon = Cast<ASamuraiWeapon>(GetOwningActor());
}
