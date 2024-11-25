#include "Minion.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

void AMinion::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::MinionWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AMinion::BeginPlay()
{
	CharacterName = ECharacterType::Minion;
	Super::BeginPlay();
}
