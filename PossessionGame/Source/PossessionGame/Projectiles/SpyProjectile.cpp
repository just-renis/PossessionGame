#include "SpyProjectile.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

float ASpyProjectile::AI_UpgradeDamageAppliance()
{
	if (!GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::SpyAbility1, EUpgradeAttribute::Special1)) return 0.f;
	const TObjectPtr<AEnemy> Proprietor = ProjectileData.GetProprietor();
	const float DamageBuff = 2.f - Proprietor->GetCurrentHp() / Proprietor->GetMaxHp();
	return DefaultDamage * DamageBuff;
}

void ASpyProjectile::AI_FinalChangesToDamage()
{
	if (!GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::SpyAbility3, EUpgradeAttribute::Special1)) return;
	if (FMath::RandRange(1, 4) == 1) Damage *= 2.f;
}
