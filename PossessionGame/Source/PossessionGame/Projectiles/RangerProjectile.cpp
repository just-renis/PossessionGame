#include "RangerProjectile.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

float ARangerProjectile::AI_UpgradeDamageAppliance()
{
	if (!GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::RangerAbility1, EUpgradeAttribute::Special1)) return 0.f;
	const int32 AbilitiesUpgradesLevel = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetCurrentLevelSumOfCharacterAbilities(ProjectileData.GetProprietor()->GetCharacterName());
	return DefaultDamage * AbilitiesUpgradesLevel * 0.1f;
}
