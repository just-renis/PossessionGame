#include "Wraith.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AWraith::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::WraithWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AWraith::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &AWraith::SetIsActionInMotion);
}

void AWraith::BeginPlay()
{
	CharacterName = ECharacterType::Wraith;
	Super::BeginPlay();
}
