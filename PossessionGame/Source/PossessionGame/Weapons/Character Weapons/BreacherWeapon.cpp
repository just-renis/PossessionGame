#include "BreacherWeapon.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ABreacherWeapon::P_Shoot()
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	if (AnimInstance)
	{
		Mesh->SetComponentTickEnabled(true);
		AnimInstance->PlayMontage(EWeaponAnimState::Shoot);
	}
	Super::P_Shoot();
}

void ABreacherWeapon::BeginPlay()
{
	Name = EWeapon::BreacherWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}
