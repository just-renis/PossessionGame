#include "Warden.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

void AWarden::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::WardenBlasterWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AWarden::BeginPlay()
{
	CharacterName = ECharacterType::Warden;
	Super::BeginPlay();
}
