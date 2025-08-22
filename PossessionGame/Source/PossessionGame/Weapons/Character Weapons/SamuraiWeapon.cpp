#include "SamuraiWeapon.h"
#include "PossessionGame/Enums/EBowState.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EPlayerInputState.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ASamuraiWeapon::P_Shoot()
{
	PlayHandsAndWeaponMontage(EHandsAnimState::Shoot, EWeaponAnimState::Shoot);
	Super::P_Shoot();
}

void ASamuraiWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(CurrentAmmo); }

void ASamuraiWeapon::P_SetStretchingState(EPlayerInputState bState)
{
	if (BowState == EBowState::Shoot || BowState == EBowState::ShootRelease) return;
	switch (bState)
	{
		case EPlayerInputState::LeftClickRelease:
			if (BowState == EBowState::StretchStart) BowState = EBowState::StretchRelease;
			else if (BowState == EBowState::StretchHold)
			{
				BowState = EBowState::Shoot;
				P_Shoot();
			}
			else return;
			break;
		case EPlayerInputState::RightClickStart:
			if (BowState == EBowState::StretchStart || BowState != EBowState::StretchRelease) BowState = EBowState::StretchRelease;
			else return;
			break;
		case EPlayerInputState::LeftClickStart: BowState = EBowState::StretchStart; break;
		default: return;
	}
	P_PlayBowMontage();
}

void ASamuraiWeapon::P_PlayBowMontage()
{
	switch (BowState)
	{
		case EBowState::StretchStart: HandsAnimInstance->IsPlaying(EHandsAnimState::StretchRelease) ? PlayHandsAndWeaponMontage(EHandsAnimState::StretchStart,
			EWeaponAnimState::StretchStart, HandsAnimInstance->GetMontagePosition(EHandsAnimState::StretchRelease)) :
			PlayHandsAndWeaponMontage(EHandsAnimState::StretchStart, EWeaponAnimState::StretchStart);
			break;
		case EBowState::StretchRelease: HandsAnimInstance->IsPlaying(EHandsAnimState::StretchStart) ? PlayHandsAndWeaponMontage(EHandsAnimState::StretchRelease,
			EWeaponAnimState::StretchRelease, HandsAnimInstance->GetMontagePosition(EHandsAnimState::StretchStart)) :
			PlayHandsAndWeaponMontage(EHandsAnimState::StretchRelease, EWeaponAnimState::StretchRelease);
			break;
		default: break;
	}
}

void ASamuraiWeapon::BeginPlay()
{
	Name = EWeapon::SamuraiWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}

void ASamuraiWeapon::PlayHandsAndWeaponMontage(EHandsAnimState HandsState, EWeaponAnimState WeaponState, float StartSeconds)
{
	HandsAnimInstance->PlayMontage(HandsState, StartSeconds);
	AnimInstance->PlayMontage(WeaponState, StartSeconds);
}

void ASamuraiWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const bool bFromFirstWeapon) { Super::ShootProjectile(EPoolObject::ArrowProjectile, false); }
